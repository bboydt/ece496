#pragma once

#include <stddef.h>

#include <neorv32.h>

typedef neorv32_uart_t FILE;

#define stdin  ((FILE*)NEROV32_UART0)
#define stdout ((FILE*)NEORV32_UART0)
#define stderr ((FILE*)NEORV32_UART0)

int vsscanf(const char *string, const char *format, va_list args);
int sscanf(const char *string, const char *format, ...);

int vfscanf(FILE *stream, const char *format, va_list args);
int fscanf(FILE *stream, const char *format, ...);

int vfprintf(FILE *stream, const char *format, va_list args);
int fprintf(FILE *stream, const char *format, ...);

int vsprintf(char *buffer, const char *format, va_list args);
int sprintf(char *buffer, const char *format, ...);

char fgetc(FILE *stream);
char *fgets(char *str, int size, FILE *stream);

int fputc(int c, FILE *stream);
int fputs(const char *str, FILE *stream);

size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
