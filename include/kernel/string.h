#ifndef __STRING_H__
#define __STRING_H__

#include <kernel/types.h>

void memset(void *dest, int val, size_t count);
void memset64(uint64_t *dest, uint64_t val, size_t count);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *str1, const char *str2);
size_t strlen(const char *str);

#endif
