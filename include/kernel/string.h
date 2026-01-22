#ifndef __STRING_H__
#define __STRING_H__

#include <kernel/types.h>

void memset(void *dest, int val, size_t count);
void memset64(uint64_t *dest, uint64_t val, size_t count);
char *strncpy(char *dst, char *src, size_t n);

#endif
