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

char *strncpy(char *dst, char *src, size_t n)
{
	while (n > 0 && *src != '\0') {
		*dst++ = *src++;
		n--;
	}

	return dst;
}
