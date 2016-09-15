#ifndef _cc_general_h_
#define _cc_general_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SYM_TABLE_PRIME 97

typedef struct cc_buf_s cc_buf_s;
typedef struct cc_sym_rec_s cc_sym_rec_s;
typedef struct cc_sym_s cc_sym_s;

struct cc_buf_s {
	size_t bsize;
	size_t size;
	void *buf;
};

struct cc_sym_rec_s {
	char *key;
	void *val;
	cc_sym_rec_s *next;
};

struct cc_sym_s  {
	unsigned size;
	cc_sym_rec_s *table[SYM_TABLE_PRIME];	
};

extern void cc_buf_init(cc_buf_s *b);
extern void cc_buf_str_addc(cc_buf_s *b, int c);
extern void cc_buf_str_addstr(cc_buf_s *buf, const char *str, size_t n);
extern void cc_buf_destroy(cc_buf_s *bf);

extern cc_buf_s cc_read_file(const char *fname);

extern char *dupstr(char *str);

extern void init_log(FILE *f);
extern void cc_log(const char *format, ...);

#define cc_log_err(fmt,...)	cc_log("ERROR:\t%u\t" fmt, lineno, __VA_ARGS__)


extern int cc_sym_insert(cc_sym_s *map, char *key, void *val);
extern void *cc_sym_lookup(cc_sym_s *map, char *key);
extern void cc_sym_delete(cc_sym_s *map, char *key);
extern void cc_sym_destroy(cc_sym_s *map);

/* 
 * The usual malloc wrappers
 */
extern void *cc_alloc(size_t len);
extern void *cc_allocz(size_t len);
extern void *cc_realloc(void *ptr, size_t len);

#endif
