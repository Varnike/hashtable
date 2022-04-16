#include "input.h"

int readNcnt(textBuff *btext)
{
	int fd = fileno(btext->file_in);
	printf("f\n");
	if(read(fd, btext->buff, btext->buffsize) != btext->buffsize) {
		ERRNUM = READ_ERR;
		return -1;
	}

	printf("BUFFSIZE = %zu\n", btext->buffsize);	

	btext->linecnt = count_lines(btext->buff, btext->buffsize); 

	printf("NLINES = %d\n", btext->linecnt);
	return btext->linecnt;
}

#define $ printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

int read_in_str(textBuff *btext) {
	printf("<\t%p\t>\n", btext->str);
	if (btext->str == NULL) {
		ERRNUM = STRUCT_PTR_ERR;
		return EOF;
	}

	if (btext->buff == NULL) {
		ERRNUM = BUFF_PTR_ERR;
		return EOF;
	}
$

	int curr_ptr  = 0;
	int curr_line = 0;
	int prev_cnt  = 0;
	int mm_id     = 0;

#define STR			btext->str
	btext->str[0] = _mm256_setzero_si256();
$
	for (int i = 0; curr_line < btext->linecnt
		       	&& i != btext->buffsize; i++) {
		if (isTrash(btext->buff[i])) {
			if (isTrash(btext->buff[curr_ptr])) {
				curr_ptr = i + 1;
				continue;
			}

			//printf("\n");
	
			while(isTrash(btext->buff[curr_ptr]))
				curr_ptr++;

			curr_ptr = i + 1;
			STR[++curr_line] = _mm256_setzero_si256();

			mm_id = 0;
			continue;
		}

		((char *) &STR[curr_line])[mm_id++] = 
			btext->buff[i];

#ifdef ONEGIN_DEBUG
		printf("%d\t%c\n", mm_id, 
			((char *) &STR[curr_line])[mm_id++]);
#endif
	}
#ifdef ONEGIN_DEBUG
	for (int i = 0; i != 32; i++)
		printf("%d <%c>\n", i, ((char *) &STR[1])[i]);

	printf("\n");
	
#endif
	return btext->linecnt;
}
#undef STR
#undef $

int isTrash(char c)
{
	return !isalpha(c) && !isdigit(c) && (c != '\'');
}

int count_lines(char *str, const size_t len)
{
	if (str == NULL)
		return EOF;
	int cnt = 0;
	int i = 0;
	for (i = 0; i != len; i++)
		if (!isTrash(str[i]))
			break;

	for ( ; i != len; i++) {
		if (isTrash(str[i]) && !isTrash(str[i-1]))
			cnt++;
	}

	return cnt;
}
