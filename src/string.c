#include <kernel/string.h>

void memset(void *dest, int val, size_t count)
{
	uint8_t *ptr = (uint8_t *) dest;
	while (count--) {
		*ptr++ = (uint8_t) val;
	}
}

void memset64(u64 *dest, u64 val, size_t count)
{
	while (count--) {
		*dest++ = val;
	}
}

char *strncpy(char *dst, const char *src, size_t n)
{
	while (n > 1 && *src != '\0') {
		*dst++ = *src++;
		n--;
	}

	dst[n] = 0;

	return dst;
}

int strcmp(const char *str1, const char *str2)
{
	while (*str1 != '\0') {
		if (*str1++ != *str2++)
			break;
	}

	return *(unsigned char*)str1 - *(unsigned char*)str2;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (*str++ != '\0') len++;
	return len;
}
