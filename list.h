#ifndef _LIST_H
#define _LIST_H

#include <stdio.h>
#include <cstring>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <stdint.h>
#include <inttypes.h>
#include <nmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include "error.h"
#include "config.h"

//#define LIST_DEBUG

#define $ printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

#define ListDump(list) _ListDump(list,  __func__, __FILE__, __LINE__);

#define VAR_NAME(v)       #v

#ifdef LIST_DEBUG
#define LIST_CHECK 					\
	{						\
		assert(list);				\
		assert(list->buff);			\
		ERRNUM = 0;				\
		ListCheck(list);			\
		if (ERRNUM) {				\
			perror(errmsg(ERRNUM));		\
			ListDump(list);			\
			assert(!"HUGE ERR!");		\
		}					\
	}
#else 
#define LIST_CHECK
#endif

//typedef int listv_t;

typedef struct HST_DATA {
	__m256i *key;
	int data;	
} listv_t;

//typedef int listv_t;

const int DEFAULT_BUFF_SIZE = 5;

/**
 * Flag of dummy node, which used for hashtable search output.
 * This is value of HST_DATA.data
 */
const int DUMMY_NODE = -1;

enum get_data_mode {
	PHYS_ADDR = 0,
	LINEAR_LOGIC_ADDR = 1 // only for linearised list!
};

struct _NODE {
	listv_t val;
	int next;
	int prev;
};

struct LIST {
	struct _NODE *buff;
	int MAX_SIZE;
	int size;
	int HEAD;
	int TAIL;
	int FREE;
	int sorted;
};

int ListCtor(LIST *list, int pos, listv_t val, 
		int size = DEFAULT_BUFF_SIZE);
int ListInsertBack(LIST *list, listv_t val);
int ListInsertFront(LIST *list, listv_t val);
int ListInsert(LIST *list, int pos, listv_t val);
int ListDelete(LIST *list, int pos);
int ListFindNodeSlowSlow(LIST *lst, int pos);
int ListDtor(LIST *list);
int ListLineariseSlowSlow(LIST *list);
_NODE *ListGetValue(LIST *list, int pos, int mode = 0);
_NODE *ListFindKey(LIST *list, __m256i *key, int len);

void _ListDump(LIST *list,  const char *srcfunc,
	       	const char *srcfile, const int line);
void DotDump(LIST *list);
int ListCheck(LIST *list);

#endif // _LIST_H
