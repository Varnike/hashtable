#include "list.h"
#include "hashtable.h"
#include "includes/onegin.h"
#include "unitest.h"

#define TEST_MODE 1
int main()
{
	unitest("djb_hahs", djb_hash);

	/*
	hashtable ht = {};
	HashTableCtor(&ht, 10, djb_hash);

	textBuff btext = {};
	$
	read_from_file(&btext, "Breaking-Bad-pilot-script.txt");
	$
	printf("linecnt = %d\n", btext.linecnt);
	
	_NODE *ntest = NULL;

	for (int i = 0; i != btext.linecnt; i++) {
#if TEST_MODE == 1
		ntest = HashTableFind(&ht, btext.str[i].strptr, 
					btext.str[i].len);
		if (ntest->val.data != DUMMY_NODE)
			HashTableInsert((LIST *)(ntest->val.key),
				HST_DATA {
					.key  = btext.str[i].strptr,
					.data = 0
			});
#else
		HashTableInsert(&ht, HST_DATA {
					.key  = btext.str[i].strptr,
					.data = 0
			}, btext.str[i].len);
#endif
	}
	
	
	char *arr[] = {"qw", "er", "ty", "ui", "op"};
	HST_DATA test_val = {
		.key = "test+",
		.data = 0
	};

	hashtable ht = {};
	HashTableCtor(&ht, 10, djb_hash);
	
	for (int i = 0; i != 5; i++) {
		HashTableInsert(&ht, test_val, 2);
		test_val.key  = arr[i];
		test_val.data = i + 1;
	}

	for (int i = 0; i != 4; i++) {	
		if (ntest->val.data == DUMMY_NODE)
			HashTableInsert((LIST *)(ntest->val.key), 
					HST_DATA {
						.key  = "qwq",
						.data = 0
					});
	}
	HashTableDump(&ht);
	

	HashTableDtor(&ht);	
	*/
}
