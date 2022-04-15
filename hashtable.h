#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <cstring>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <stdint.h>
#include <inttypes.h>
#include "list.h"

#define	CHECK_(rel, errcode) 	if (rel) return ERRNUM = errcode;
const int HS_SIZE = 200;
const int LIST_DEF_SIZE = 100;

struct hashtable {
	LIST *table = NULL;
	int size = 0;
	uint32_t (*hash)(const char *str, size_t len);
};

int HashTableCtor(hashtable *ht, int size,
	       	uint32_t (*hash)(const char *str, size_t len));
int HashTableDtor(hashtable *ht);
int HashTableInsert(hashtable *ht, HST_DATA node, size_t keylen);
int HashTableInsert(LIST *list, HST_DATA node);
// TODO int HashTableDelete(hashtable *ht, char *key, size_t keylen);
/**
 * NOTE: never use list function push front!
 *
 */
_NODE* HashTableFind(hashtable *ht, __m256i *key, size_t keylen);
int HashTableDump(hashtable *ht);
int _HashTableCheck(hashtable *ht);

uint32_t djb_hash(const char* data, size_t length);



#endif
