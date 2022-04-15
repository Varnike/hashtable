#include "list.h"

static void init_list(LIST *list);
static void init_free_space(LIST *list, int start = 0);
static void add_free_node(LIST *list, int pos);
static int get_free_node(LIST *list);
static void set_list_node(LIST *list, int pos, listv_t val, int next, int prev);
static int list_resize(LIST *list);
static int list_broken(void);

int ListCtor(LIST *list, int pos, listv_t val, int size)
{
	assert(list);
	
	if (list->buff != NULL) {
		return ERRNUM = LIST_CTOR_ERR;
	}
	
	list->MAX_SIZE = size;
	list->buff = (_NODE*)calloc(sizeof(_NODE), list->MAX_SIZE);
	
	init_list(list);
	set_list_node(list, pos, val, 0, 0);
	init_free_space(list);
	
	list->size = 1;
	list->HEAD = pos;
	list->TAIL = pos;

	LIST_CHECK;
	return 0;
}

int ListDtor(LIST *list)
{
	LIST_CHECK;

	free(list->buff);

	return 0;
}

_NODE *ListGetValue(LIST *list, int pos, int mode)
{
	LIST_CHECK;
	if (mode == LINEAR_LOGIC_ADDR) {
		if (list->sorted)
			return (list->buff + pos);
		else {
			ERRNUM = LIST_LOGIC_ADDR_ERR;
			return NULL;
		}
	} else {
		return (list->buff + pos);
	}
}

int ListInsertBack(LIST *list, listv_t val)
{
	LIST_CHECK;

	int lnext = get_free_node(list);

	if (lnext < 0)
		return -1;

	list->sorted = 0;	
	list->size++;
	set_list_node(list, lnext, val, 0, list->TAIL);
	
	list->buff[list->TAIL].next = lnext;
	list->TAIL = lnext;
	
	LIST_CHECK;
	return lnext;
}

int ListInsertFront(LIST *list, listv_t val)
{
	LIST_CHECK;

	int lnext = get_free_node(list);
	if (lnext < 0) 
		return -1;

	list->sorted = 0;
	list->size++;
	set_list_node(list, lnext, val, list->HEAD, 0);
	
	list->buff[list->HEAD].prev = lnext;
	list->HEAD = lnext; 

	LIST_CHECK;                     
	
	return lnext;
}                                          

int ListInsert(LIST *list, int pos, listv_t val)
{
	LIST_CHECK;
	
	int lnext = get_free_node(list);
	if (lnext < 0)
		return -1;

	list->sorted = 0;
	list->size++;

	/* setup new node */
	set_list_node(list, lnext, val, list->buff[pos].next, pos);
	
	/* update old ones */
	list->buff[list->buff[pos].next].prev = lnext;
	list->buff[pos].next  = lnext;
	
	LIST_CHECK;

	return lnext;
}

int ListDelete(LIST *list, int pos)
{
	LIST_CHECK;

	if (pos <= 0) {
		ERRNUM = LIST_INV_POS_ERR;
		return -1;
	}	
//кольцевой список
	list->sorted = 0;	
	list->size--;

	if (pos == list->TAIL || pos == list->HEAD) {
		if (pos == list->TAIL) {
			list->TAIL = list->buff[pos].prev;
		
			list->buff[list->TAIL].next = 0;
		} else {
			list->HEAD = list->buff[pos].next;
                                  
			list->buff[list->HEAD].prev = 0;  
		}

		set_list_node(list, pos, HST_DATA{}, -1, -1);
		add_free_node(list, pos);
		return 0;
	}

	int lprev = list->buff[pos].prev;
	int lnext = list->buff[pos].next;

	list->buff[lprev].next = lnext;
	list->buff[lnext].prev = lprev;	

	set_list_node(list, pos, HST_DATA{}, -1, -1);
	add_free_node(list, pos);

	LIST_CHECK;
	return 0;
}

int ListLineariseSlowSlow(LIST *list)
{
	LIST_CHECK;

	if (list->sorted == 1)
		return 0;

	_NODE *newbuff = list->buff;
	list->buff = (_NODE*)calloc(sizeof(_NODE), list->MAX_SIZE);	
	
	if (list->buff == NULL)
		return ERRNUM = CALLOC_ERR;

	init_list(list);
	
	int node_cnt = 0;
	int it = list->HEAD;
	
	while (it != 0 && node_cnt++ != list->MAX_SIZE) {
		set_list_node(list, node_cnt, newbuff[it].val,
			       	node_cnt + 1, node_cnt - 1);
		it = newbuff[it].next;
	}
	
	list->HEAD = 1;
	list->TAIL = node_cnt;
	list->buff[1].prev = 0;
	list->buff[list->TAIL].next = 0;
	list->sorted = 1;
	
	free(newbuff);
	
	init_free_space(list, list->size + 1);
		
	LIST_CHECK;
	return 0;
}

int ListFindNodeSlowSlow(LIST *list, int logical_pos)
{
	LIST_CHECK;

	for (int ph_pos = 0; ph_pos != list->MAX_SIZE; ph_pos++) {
		if (list->buff[ph_pos].next == logical_pos)
			return ph_pos;
	}
	
	ERRNUM = LIST_NODE_NOT_FOUND;

	return -1;
}

_NODE *ListFindKey(LIST *list, __m256i *key, int len)
{
	LIST_CHECK;
	if (!key) {
		ERRNUM = LIST_NULLPTR_ERR;
		return NULL;
	}

	int it = list->buff[list->HEAD].next;	// skip dummy node
	do {
		if (!list->buff[it].val.key) {
			printf("NULL DATA!");
			break;
		}

		__m256i cmp = _mm256_cmpeq_epi8(*list->buff[it].val.key,
						*key);
		if (!_mm256_movemask_epi8(cmp))
			//strncmp(list->buff[it].val.key, key, len) == 0)
			return list->buff + it;

		it = list->buff[it].next;
	} while(it != 0);

	return list->buff + 1; 			// dummy node
}
#if 0
void _ListDump
(LIST *list, const char *srcfunc, const char *srcfile, const int line)
{
	//LIST_CHECK;
	assert(0);

	assert(list);
	assert(list->buff);
	
	FILE *file = fopen("dump1.html", "w");
	
	int list_no = 1;
	int it = list->HEAD;
	
	if (file == NULL) {
		perror("Can't open/create log file\n");
		goto err_close_file;
	}
	
	fprintf(file, "<html> <h1>List dump</h1> <pre>\n");
	if (ERRNUM) {
		fprintf(file,"ERROR : %s\n", errmsg(ERRNUM));
	}
	
	fprintf(file, "List<%s>:",typeid(listv_t).name());

	if (ERRNUM)
		fprintf(file, "ERROR!");
	else
		fprintf(file, "ok!");

	fprintf(file, "; \"%s\" called from %s at %s (%d)\n{\n",
		       VAR_NAME(list), srcfunc, srcfile, line);

	fprintf(file, "\tHEAD = %d, TAIL = %d, FREE = %d,"
			" size = %d, capacity = %d\nPHYSICAL LIST:\n",
		       	list->HEAD, list->TAIL, 
			list->FREE, list->size, list->MAX_SIZE);
	
	for (int i = 0; i != list->MAX_SIZE; i++) 
		fprintf(file, "\t[  %d  ] prev[ %d ] "
				"-- next[ %d ] -- data[ %s ]\n", 
				i, list->buff[i].prev, 
				list->buff[i].next, 
				list->buff[i].val.key);
	
	if (list_broken()) {
		fprintf(file, "\tcant't print logical list:"
				" list broken!\n");
		goto err_close_file;
	}
	
	fprintf(file, "LOGICAL LIST:\n");
	

	do {
		fprintf(file, "\t[  %d  ] prev[ %d ] -- next[ %d ]"
				" -- val[ %d ]\n", list_no++, 
				list->buff[it].prev,	
				list->buff[it].next, 
				list->buff[it].val);

		it = list->buff[it].next;
	} while(it != 0);

	fprintf(file, "}\n\n</pre> <img src=\"dump/dump.svg\""
			" alt=\"ListDump\">\n <\html>\n");

err_close_file:
	if (file != stdout) {
		fclose(file);
	}
}

void DotDump(LIST *list)
{
	assert(0);

	LIST_CHECK;

	assert(list);
	assert(list->buff);
	
	int it = list->TAIL;
	
	FILE *fout = NULL; 
	fout = fopen("dump/dump.dot", "w");
	assert(fout);

	fprintf(fout, "digraph dump_graph {\n\trankdir=LR; \n");
		
	for (int i = 1; i != list->MAX_SIZE; i++) {	
		fprintf(fout, "\tstruct%d [shape=box3d\n\t\
				label=\n\t<\n\t<table border=\"0\" cellspacing=\"0\">\n\
				\t\t<tr><td PORT=\"port0\" border=\"1\"  bgcolor=\"lightskyblue\">%d</td></tr>\n\
				\t\t<tr><td PORT=\"port1\" border=\"1\" bgcolor=\"grey69\">next: %d</td></tr>\n\
				\t\t<tr><td PORT=\"port2\" border=\"1\" bgcolor=\"grey69\">prev: %d</td></tr>\n\
				\t\t<tr><td PORT=\"port3\" border=\"1\" bgcolor=\"grey69\">data: %d</td></tr>\n\
				\t</table>>];\n",
	                i, i, list->buff[i].next, list->buff[i].prev,
		       	list->buff[i].val.data);
		
		if (i == list->MAX_SIZE - 1) {
			break;
		}
		fprintf(fout, "\tstruct%d:port0 -> struct%d:port0[style=invis];\n", i, i+1);


		if (list->buff[i].prev != -1) {
			if (list->buff[i].next != 0)
        			fprintf(fout, "\tstruct%d:port1 -> struct%d:port1[style=bold, arrowhead=vee,\
					color=forestgreen];\n", i, list->buff[i].next);
			if (!list->buff[i].prev == 0)
				fprintf(fout, "\tstruct%d:port2 -> struct%d:port2[style=dashed, arrowhead=onormal,\
					color=grey];\n", i, list->buff[i].prev);
		} else {
			if (list->buff[i].next != 0)
				fprintf(fout, "\tstruct%d:port1 -> struct%d:port1;\n", 
					i, list->buff[i].next);
		}

	
	//	it = list->buff[it].next;
	}

	fprintf(fout, "\tfree[fillcolor=\"pink\",style=filled, shape=doubleoctagon, label=\"FREE = %d\"];\n", 
			list->FREE);
	if (list->FREE != 0)
		fprintf(fout, "\tfree -> struct%d:port1;\n", list->FREE);

	fprintf(fout, "\thead[fillcolor=\"pink\", style=filled, shape=doubleoctagon, label=\"HEAD = %d\"];\n", 
			list->HEAD);
	fprintf(fout, "\thead -> struct%d:port1;\n", list->HEAD);

	fprintf(fout, "\ttail[fillcolor=\"pink\", style=filled, shape=doubleoctagon, label=\"TAIL = %d\"];\n", 
			list->TAIL);
	fprintf(fout, "\ttail -> struct%d:port1;\n", list->TAIL);


	fprintf(fout, "}\n");

	if (fout != stdout)
		fclose(fout);

	system("dot -Tsvg dump/dump.dot -o dump/dump.svg");
}
#endif
static void init_list(LIST *list)
{
	assert(list);

	for (int i = 1; i != list->MAX_SIZE; i++) {
		list->buff[i].next = -1;
		list->buff[i].prev = -1;
	}
		
	set_list_node(list, 0, HST_DATA{}, 0, 0);
}

int ListCheck(LIST *list)
{
	assert(list);
	assert(list->buff);

	if (list->buff[list->HEAD].next < 0  
			|| (list->buff[list->HEAD].prev != 0 
				&& list->size != 1))
		return ERRNUM = LIST_BAD_HEAD_ERR;

	if (list->buff[list->TAIL].next != 0)
		return ERRNUM = LIST_BAD_TAIL_ERR;

	int it = list->HEAD;

	for (int list_it = 1; list_it != list->MAX_SIZE; ) {
		it = list->buff[it].next;
		list_it++;
	
		/* check if it is valid */
		if (it < 0) {
			return ERRNUM = LIST_NODE_BAD_NEXT;
		} else if (it == 0) {
			return LIST_BROKEN;	
		}

		if (list->buff[it].next == 0) {
			if (list_it == list->size && it == list->TAIL) {
				break;
			} else {
				printf("ERROR on node %d: count is %d, size is %d\n",
						it, list_it, list->size);
	
				return ERRNUM = LIST_BROKEN;
			}
		}
	
		if (list_it == list->size)
			return ERRNUM = LIST_BROKEN; 


		if (list->buff[it].prev < 0) 
			return ERRNUM = LIST_NODE_BAD_PREV;
		/* Check connection between neighbor nodes */
		if (list->buff[list->buff[it].prev].next != it) {
			printf("ERROR on node %d: prev is %d, next[prev] is %d\n", 
					it, list->buff[it].prev, list->buff[list->buff[it].prev].next);
			return ERRNUM = LIST_CONNECTION_ERR;
		}
	}

	/* Check free list */
	if (list->FREE < 0)
		return ERRNUM = LIST_BAD_FREE_ERR;

	it = list->FREE; 

	for (int list_cnt = 1; list_cnt != list->MAX_SIZE && list->buff[it].next; ) {
		it = list->buff[it].next;
		list_cnt++;

		if (it < 0)
			return ERRNUM = LIST_FREE_LIST_BROKEN;

		if (list_cnt > list->MAX_SIZE - list->size)
			return ERRNUM = LIST_FREE_LIST_BROKEN;
		
		if (list->buff[it].prev != -1) {
			printf("node %d: next = %d, prev = %d\n", it, list->buff[it].next, list->buff[it].prev);
			return ERRNUM = LIST_BAD_FREE_NODE;
		}
	}	

	return ERRNUM = NO_ERR;
}

static void set_list_node(LIST *list, int pos, listv_t val, int next, int prev)
{
	assert(list);
	assert(list->buff);

	list->buff[pos].val  = val; 
	list->buff[pos].next = next;
	list->buff[pos].prev = prev;
	
}

static void add_free_node(LIST *list, int pos)
{
	assert(list);
	assert(list->buff);

	set_list_node(list, pos, HST_DATA{}, list->FREE, -1);

	list->FREE = pos;
}

static int get_free_node(LIST *list)
{
	assert(list);
	assert(list->buff);
	
	if (list->FREE == 0) {
		list_resize(list);
	}

	//DotDump(list);

	int free_node = list->FREE;
	list->FREE = list->buff[list->FREE].next;
	
	RUN_PRINTF("free node found : %d.\n", free_node);
	return free_node;
}

static void init_free_space(LIST *list, int start)
{
	assert(list);
	assert(list->buff);

	int it = start;

	for (it = start; it != list->MAX_SIZE; it++) {
		if (list->buff[it].prev == -1) {
			list->FREE = it;
			break;
		}
	}
	
//	printf("free is : %d\n", list->FREE);
	int prev = list->FREE;
	
	for ( ; it != list->MAX_SIZE; it++) {
		if (list->buff[it].prev == -1) {
//			printf("it is : %d\n", it);
			list->buff[prev].next = it;
			prev = it;
		}
	}

	list->buff[prev].next = 0;
}

int list_resize(LIST *list)
{
	LIST_CHECK;
	
	ListLineariseSlowSlow(list);
	
	if (ERRNUM)
		return ERRNUM;
	
	list->buff = (_NODE *)reallocarray(list->buff, 2 * list->MAX_SIZE,  sizeof(_NODE));

	if (list->buff == NULL)
		return ERRNUM = REALL_ERR;
	
	list->MAX_SIZE *= 2;

	for (int list_it = list->size + 1; list_it != list->MAX_SIZE; list_it++) {
		RUN_PRINTF("f		%d\n", list_it);
		list->buff[list_it].prev = -1;
		list->buff[list_it].val  = HST_DATA{};
	}	
	
	init_free_space(list, list->size + 1);;
	
	//DotDump(list);
	LIST_CHECK;
	return 0;
}

int list_broken(void) {
	return (ERRNUM == LIST_CONNECTION_ERR || ERRNUM == LIST_BROKEN || ERRNUM == LIST_NODE_BAD_NEXT
			|| ERRNUM == LIST_BAD_TAIL_ERR || ERRNUM == LIST_NODE_BAD_PREV);
}
//TODO HTM log
