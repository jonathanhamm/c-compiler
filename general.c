#include "general.h"
#include <string.h>
#include <stdarg.h>

enum {
	INIT_BUF_SIZE = 512
};

static FILE *logfile; 

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
	size_t i;
	size_t size = b->size, 
	       bsize = b->bsize,
	       nsize = size + n;
	char *buf = b->buf, *dst = &buf[size];
	
	if(nsize >= bsize) {
		do {
			bsize *= 2;
		} 
		while(bsize <= nsize);
		b->bsize = bsize;
		buf = cc_realloc(buf, bsize);
		b->buf = buf;
	}
	for(i = 0; i < n; i++) {
		*dst++ = *str++;	
	}
	b->size = nsize;
}

void cc_buf_destroy(cc_buf_s *b) {
	free(b->buf);
}

cc_buf_s cc_read_file(const char *fname) {
	int c;
	FILE *f;
	cc_buf_s buf;
	
	f = fopen(fname, "r");
	if(f) {
		cc_buf_init(&buf);
		
		while((c = fgetc(f)) != EOF)
			cc_buf_str_addc(&buf, c);
		cc_buf_str_addc(&buf, '\0');
		fclose(f);
	}
	else {
		perror("Error opening file");
		buf.size = 0;
		buf.bsize = 0;
		buf.buf = NULL;
	}
	return buf;
}

char *dupstr(char *str) {
	size_t size = strlen(str) + 1;
	char *nstr = cc_alloc(size);
	strcpy(nstr, str);
	return nstr;
}

void init_log(FILE *f) {
	logfile = f;
}

void cc_log(const char *format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(logfile, format, args);
	va_end(args);
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

