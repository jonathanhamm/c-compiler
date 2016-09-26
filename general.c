#include "general.h"
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

enum {
	INIT_BUF_SIZE = 512
};

static FILE *logfile; 

static cc_sym_rec_s *cc_sym_rec_s_(char *key, void *val);
static unsigned pjwhash(const char *key);

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

void cc_buf_clear(cc_buf_s *buf) {
	char *bptr = buf->buf;
	buf->size = 0;
	*bptr = '\0';
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

void cc_sym_init(cc_sym_s *map) {
	int i;
	cc_sym_rec_s **pt = map->table;

	map->size = 0;
	for(i = 0; i < SYM_TABLE_PRIME; i++, pt++) {
		*pt = NULL;
	}	
}

int cc_sym_insert(cc_sym_s *map, char *key, void *val) {
	cc_sym_rec_s **prec = &map->table[pjwhash(key)], 
		     *rec = *prec;

	cc_sym_rec_s *nrec = cc_sym_rec_s_(key, val);
	
	if(rec) {
		for(;;) {
			if(!strcmp(rec->key, key)) {
				free(nrec);
				return -1;
			}
			else if(rec->next) {
				rec = rec->next;
			}
			else {
				break;
			}
		}
		rec->next = nrec;
	}
	else {
		*prec = nrec;
	}
	return 0;
}

void *cc_sym_lookup(cc_sym_s *map, char *key) {
	cc_sym_rec_s *rec = map->table[pjwhash(key)];

	if(!strcmp(rec->key, key)) {
		return rec->val;
	}
	else {
		do {
			if(!strcmp(rec->key, key)) {
				return rec->val;
			}
			else {
				rec = rec->next;
			}
		} 
		while(rec);
		return NULL;
	}
}

void cc_sym_delete(cc_sym_s *map, char *key) {
	cc_sym_rec_s **prec = &map->table[pjwhash(key)],
		       *rec = *prec,
		       *bck = NULL;
	if(rec) {
		do {
			if(!strcmp(rec->key, key)) {
				if(bck) {
					bck->next = rec->next;
				}
				else {
					*prec = NULL;
				}
				free(rec);
			}
			else {
				bck = rec;
				rec = rec->next;
			}
		}
		while(rec->next);
	}
}

void cc_sym_foreach(cc_sym_s *map, void (*f)(cc_sym_rec_s *)) {
	int i;
	cc_sym_rec_s **pt = map->table, *rec;

	for(i = 0; i < SYM_TABLE_PRIME; i++, pt++) {
		rec = *pt;
		if(rec) {
			do {
				f(rec);
				rec = rec->next;
			} while(rec);
		}
	}
}

void cc_sym_destroy(cc_sym_s *map) {
	int i;
	cc_sym_rec_s **ptr = map->table;

	for(i = 0; i < SYM_TABLE_PRIME; i++, ptr++) {
		cc_sym_rec_s *iter = *ptr;
		if(iter) {
			cc_sym_rec_s *bck = iter;
			while(bck) {
				bck = iter->next;
				free(iter);
				iter = bck;
			}
		}
	}
}

cc_sym_rec_s *cc_sym_rec_s_(char *key, void *val) {
	cc_sym_rec_s *rec = cc_alloc(sizeof *rec);
	rec->key = key;
	rec->val= val;
	rec->next = NULL;
	return rec;
}

unsigned pjwhash(const char *key) {
	const char *str = key;
	unsigned h = 0, g;

	while(*str) {
		h = (h << 4) + *str++;
		if((g = h & (unsigned)0xf0000000)) {
			h = (h ^ (g << 24)) ^ g;
		}
	}
	return h % SYM_TABLE_PRIME;
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

