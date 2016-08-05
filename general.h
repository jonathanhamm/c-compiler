#ifndef _cc_general_h_
#define _cc_general_h_

#include <stdlib.h>

typedef struct cc_buf_s cc_buf_s;

struct cc_buf_s {
	size_t bsize;
	size_t size;
	void *buf;
};

extern void cc_buf_init(cc_buf_s *b);
extern void cc_buf_str_addc(cc_buf_s *b, int c);
extern void cc_buf_str_addstr(cc_buf_s *buf, const char *str, size_t n);
extern void cc_buf_destroy(cc_buf_s *bf);

extern cc_buf_s cc_read_file(const char *fname);

/* 
 * The usual malloc wrappers
 */
extern void *cc_alloc(size_t len);
extern void *cc_allocz(size_t len);
extern void *cc_realloc(void *ptr, size_t len);

#endif
