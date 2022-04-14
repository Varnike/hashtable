#include "unitest.h"

#define TEST_MODE 1
int unitest(char *hash_name, uint32_t (*hash)(const char *str, size_t len))
{
	hashtable ht = {};
	textBuff btext = {};

	HashTableCtor(&ht, 20, djb_hash);

	read_from_file(&btext, "breaking_bad.txt");
	printf("linecnt = %d\n", btext.linecnt);
 
	_NODE *ntest = NULL;

	for (int i = 0; i != btext.linecnt; i++) {

		printf("%d\n", i);
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

	char str[100] = {0};
	int len = 0;

	while (scanf("%s", &str) && (len = strlen(str)) != 1) {
		ntest = HashTableFind(&ht, str, len);
		printf("~[%s] is %d\n", str, ntest->val.data);
	}

	cnt_collisions(&ht, hash_name);

	HashTableDtor(&ht);
	onegin_dtor(&btext);

	return ERRNUM;
}

int cnt_collisions(hashtable *ht, char *filename)
{
	FILE *file = fopen(filename,"w");

	for (int it = 0; it != ht->size; it++)
		fprintf(file, "%d\t%d\n", it + 1,
			       	ht->table[it].size);

	fclose(file);
}
