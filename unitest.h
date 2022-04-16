#ifndef UNITEST_H
#define UNITEST_H

#include "list.h"
#include "hashtable.h"
#include "includes/onegin.h"

const int HT_TEST_SIZE = 20;

uint32_t achr_hash(const char *str, size_t len);
uint32_t len_hash(const char *str, size_t len);
uint32_t asum_hash(const char *str, size_t len);

int unitest(char *hash_name, 
		uint32_t (*hash)(const char *str, size_t len));
void test_search(hashtable *ht, textBuff *btext);
void cnt_collisions(hashtable *ht, char *filename);
void get_words_cnt(hashtable *ht);

#endif
