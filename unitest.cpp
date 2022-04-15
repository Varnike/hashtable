#include "unitest.h"

#define TEST_MODE 1

int unitest(char *hash_name, uint32_t (*hash)(const char *str, size_t len))
{
	hashtable ht = {};
	textBuff btext = {};

	HashTableCtor(&ht, 20, hash);

	read_from_file(&btext, "breaking_bad.txt");
	printf("linecnt = %d\n", btext.linecnt);
 
	_NODE *ntest = NULL;

	for (int i = 0; i != btext.linecnt; i++) {

		RUN_PRINTF("%d\n", i);
#if TEST_MODE == 1
		ntest = HashTableFind(&ht,
			       	btext.str[i].strptr, 
				btext.str[i].len);
		if (ntest->val.data == DUMMY_NODE) {
			HashTableInsert((LIST *)(ntest->val.key),
			HST_DATA {
				.key  = btext.str[i].strptr,
				.data = 0
			});
		} else {
			ntest->val.data++;
		}
#else
		HashTableInsert(&ht, HST_DATA {
				.key  = btext.str[i].strptr,
				.data = 0
			}, btext.str[i].len);
#endif

	}

	//get_words_cnt(&ht);
	cnt_collisions(&ht, hash_name);

	HashTableDtor(&ht);
	onegin_dtor(&btext);

	return ERRNUM;
}

void cnt_collisions(hashtable *ht, char *filename)
{
	FILE *file = fopen(filename,"w");

	for (int it = 0; it != ht->size; it++)
		fprintf(file, "%d\t%d\n", it,
			       	ht->table[it].size - 1);

	fclose(file);
}

void get_words_cnt(hashtable *ht)
{ 
	char str[100] = {0};
	int len = 0;

	_NODE *ntest = NULL;

	while (scanf("%s%n", &str, &len) && *str != '!') {
		if (len == 1) 			// TODO ?
			break;
		ntest = HashTableFind(ht, str, len - 1);
		printf("~[%s] is %d\n", str, ntest->val.data);
	}
}

uint32_t achr_hash(const char *str, size_t len)
{
	return (uint32_t)*str;
}


uint32_t len_hash(const char *str, size_t len)
{
	return (uint32_t)len;
}

uint32_t asum_hash(const char *str, size_t len)
{
	uint32_t sum = 0;

	for (int i = 0; i < len; sum += str[i++])
		;

	return sum;
}
