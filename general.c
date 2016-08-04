#include "general.h"
#include <stdio.h>
#include <string.h>

enum {
	INIT_BUF_SIZE = 512
};

void cc_buf_init(cc_buf_s *b) {
	b->size = 0;
	b->bsize = INIT_BUF_SIZE;
	b->buf = cc_alloc(INIT_BUF_SIZE);
}

void cc_buf_str_addc(cc_buf_s *b, int c) {
	char *bptr = b->buf;
	if(b->size == b->bsize) {
		b->bsize *= 2;
		bptr = cc_realloc(bptr, b->bsize);
		b->buf = bptr;
	}
	bptr[b->size++] = c;
}

void cc_buf_str_addstr(cc_buf_s *b, const char *str, size_t n) {
	char *buf = b->buf;
	size_t size = b->size, 
	       bsize = b->bsize,
	       nsize = size + n;

	if(nsize >= bsize) {
		do {
			bsize *= 2;
		} 
		while(bsize <= nsize);
		b->bsize = bsize;
		buf = cc_realloc(buf, bsize);
		b->buf = buf;
	}
	strcpy(&buf[size], str);
	b->size = nsize;
}

void cc_buf_destroy(cc_buf_s *b) {
	free(b->buf);
}

void *cc_alloc(size_t len) {
	void *p = malloc(len);
	if(!p) {
		perror("Memory Allocation Error - malloc()");
	}
	return p;
}

void *cc_allocz(size_t len) {
	void *p = calloc(1, len);	
	if(!p) {	
		perror("Memory Allocation Error - calloc()");
	}
	return p;
}

void *cc_realloc(void *ptr, size_t len) {
	void *np = realloc(ptr, len);
	if(!np) {	
		perror("Memory Allocation Error - realloc()");
	}
	return np;
}

