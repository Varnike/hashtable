#include "hashtable.h"

uint32_t djb_hash(const char* data, size_t length);
static uint64_t crc_hash(__m256i *src);

int HashTableCtor(hashtable *ht, int size,
	       	uint32_t (*hash)(const char *str, size_t len))
{
	if (ht == NULL)
		return ERRNUM = HT_NULLPTR_ERR;

	ht->table = (LIST *)calloc(size, sizeof(LIST));
	ht->hash  = hash;
	
	assert(ht->table);
	ht->size  = size;

	for (int it = 0; it < size; it++) {
		
		ListCtor(&ht->table[it], 1, 
			HST_DATA{ // TODO bad
				.key = (__m256i *) &ht->table[it],
				.data = DUMMY_NODE
			}, LIST_DEF_SIZE);
	}

	return ERRNUM;
}

int HashTableDtor(hashtable *ht)
{
	assert(ht);
	assert(ht->table);
	
	for (int it = 0; it != ht->size; it++) {
		ListDtor(&ht->table[it]);
		if (ERRNUM)
			break;
	}

	free(ht->table);
	
	return 0;
}

int HashTableInsert(hashtable *ht, HST_DATA node, size_t keylen)
{
	assert(ht && ht->table && node.key);

	int pos = crc_hash(node.key) % ht->size;
	ListInsertBack(ht->table + pos, node);

	
	return 0;
}

int HashTableInsert(LIST *list, HST_DATA node)
{
	if (list == NULL)
		return ERRNUM = HT_NULL_LIST;

	ListInsertBack(list, node);

	return 0;
}

_NODE *HashTableFind(hashtable *ht, __m256i *key, size_t keylen)
{
	int pos = crc_hash(key) % ht->size;

	RUN_PRINTF("POSITION = %d\n", pos);
	
	_NODE *node_found = ListFindKey(ht->table + pos, *key, keylen);
	
	if (node_found->val.data != DUMMY_NODE) {
		RUN_PRINTF("[node found]\tkey: \"%s\","
				" data: \"%d\"\n", 
				(char*) node_found->val.key, 
				node_found->val.data);
	} else {
		RUN_PRINTF("Node not found\tkey: \"%s\"\n",
				(char*) key);
	}

	return node_found;
}

int _HashTableCheck(hashtable *ht)
{
	do {
		if (!ht) {
			ERRNUM = HT_NULLPTR_ERR;
			break;
		}
		if (!ht->table) {
			ERRNUM = HT_NULL_BUFF;
			break;
		}
		for (int it = 0; it != ht->size; it++) {
			if (!(ht->table + it)) {
				ERRNUM = HT_NULL_LIST;
				break;
			}

			if (ListCheck(ht->table + it))
				break;
		}
	} while (0);

	printf("%s\n", errmsg(ERRNUM));
	return ERRNUM;
}

int HashTableDump(hashtable *ht)
{
	assert(0); /* for debugging */

	assert(ht || ht->table);
	
	FILE *fout = fopen("dump/ht.dot","w");
	assert(fout);

	fprintf(fout, 
		"digraph g {\n"
		"\tfontname=\"Helvetica,Arial,sans-serif\"\n"
		"\tnode [fontname=\"Helvetica,Arial,sans-serif\"]\n"
		"\tedge [fontname=\"Helvetica,Arial,sans-serif\"]\n"
		"\tgraph [\n"
		"\t	rankdir = \"LR\"\n"
		"\t];\n"
		"\tnode [\n"
		"\t	fontsize = \"16\"\n"
		"\t	shape = \"ellipse\"\n"
		"\t];\n"
		"\tedge [\n"
		"\t];\n"
		"\t\"table\" [\n"
		"\t	label = \""
	);
	
	int it = 0;

	for ( ; it != ht->size - 1; it++)
		fprintf(fout, "<f%d> %d | | |", it, it);
	
	fprintf(fout, "<f%d> %d\"\n\t\tshape = \"record\"\n\t];\n",
		       	it, it);
#define iLIST	ht->table[it]
	for (it = 0; it != ht->size; it++) {
		for (int lit = 0; lit != iLIST.size; lit++) {
			fprintf(fout, 
				"\t\"node%p_%d\" [\n"
				"\t\tlabel = \"<f0> %d | %s | %d\"\n"
				"\t\tshape = \"record\"\n\t];\n",
				&iLIST,lit, lit, 
				iLIST.buff[lit + 1].val.key,
			       	iLIST.buff[lit + 1].val.data);
			if (iLIST.buff[lit].next)
				fprintf(fout,
				"\"node%p_%d\":f0 -> "
				"\"node%p_%d\":f0\n",
				&iLIST, lit - 1, &iLIST, 
				iLIST.buff[lit].next - 1);
		}
		fprintf(fout, "\t\"table\":<f%d> ->"
				" \"node%p_0\":<f0>\n",
				it, &iLIST);
	}
#undef iLIST

	fprintf(fout, "}\n");
	fclose(fout);

	system("dot -Tsvg dump/ht.dot -o dump/ht.svg");
}

uint32_t djb_hash(const char* data, size_t length)
{
	unsigned int hash = 5381; // MAGIC NUMBER
	
	for (int i = 0; i < length; ++i) {
		hash = (hash << 5) + hash + data[i];
	}

	return hash;
}

static uint64_t crc_hash(__m256i *src)
{
	uint64_t answ = 0;

	for (int i = 0; i != 4; i++)
		answ = _mm_crc32_u64(answ, ((uint64_t *)src)[i]);

	return answ;
}
