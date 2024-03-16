#pragma once

#include <stddef.h>

#include <neorv32.h>

#define stdin NEROV32_UART0
#define stdout NEORV32_UART0
#define stderr NEORV32_UART0

static inline char fgetc(neorv32_uart_t *stream)
{
    return neorv32_uart_getc(stream);
}

static inline char *fgets(char *str, int size, neorv32_uart_t *stream)
{
    int size_read = 0;
    while (size_read <= size - 1)
    {
        char c = neorv32_uart_getc(stream);
        str[size_read++] = c;

        if (c == '\n')
            break;
    }
    str[size_read] = '\0';
    return str;
}

static inline int vfprintf(neorv32_uart_t *stream, const char *format, va_list args)
{
    neorv32_uart_vprintf(stream, format, args);
    return 0; // @todo
}

static inline int vfscanf(neorv32_uart_t *stream, const char *format, va_list args)
{
    return 0;
}

static inline int fprintf(neorv32_uart_t *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

static inline int fputc(int c, neorv32_uart_t *stream)
{
    neorv32_uart_putc(stream, c);
    return c;
}

static inline int fputs(const char *str, neorv32_uart_t *stream)
{
    neorv32_uart_puts(stream, str);
    return 1;
}

static inline size_t fread(void *ptr, size_t size, size_t nitems, neorv32_uart_t *stream)
{
    for (size_t i = 0; i < size * nitems; i++)
    {
        ((char*)ptr)[i] = fgetc(stream);
    }
    return size * nitems;
}

static inline int fscanf(neorv32_uart_t *stream, const char *format, ...)
{
    return 0;
}

static inline size_t fwrite(const void *ptr, size_t size, size_t nitems, neorv32_uart_t *stream)
{
    for (size_t i = 0; i < size * nitems; i++)
    {
        fputc(((char*)ptr)[i], stream);
    }
    return size * nitems;
}

