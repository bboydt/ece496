#include <string.h>

void *memcpy(void *dst, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((unsigned char*)dst)[i] = ((const unsigned char*)src)[i];
    }
    return dst;
}

void *memset(void *dst, int value, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        ((unsigned char*)dst)[i] = (unsigned char)value;
    }
    return dst;
}
