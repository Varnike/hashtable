#ifndef UNITEST_H
#define UNITEST_H

#include "list.h"
#include "hashtable.h"
#include "includes/onegin.h"

const int HT_TEST_SIZE = 10;

int unitest(char *hash_name, uint32_t (*hash)(const char *str, size_t len));
int cnt_collisions(hashtable *ht, char *filename);

#endif
