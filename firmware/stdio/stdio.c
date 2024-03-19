#include <stdio.h>


static void __uart_itoa(uint32_t x, char *res) {

  static const char numbers[] = "0123456789";
  char buffer1[11];
  uint16_t i, j;

  buffer1[10] = '\0';
  res[10] = '\0';

  // convert
  for (i=0; i<10; i++) {
    buffer1[i] = numbers[x%10];
    x /= 10;
  }

  // delete 'leading' zeros
  for (i=9; i!=0; i--) {
    if (buffer1[i] == '0')
      buffer1[i] = '\0';
    else
      break;
  }

  // reverse
  j = 0;
  do {
    if (buffer1[i] != '\0')
      res[j++] = buffer1[i];
  } while (i--);

  res[j] = '\0'; // terminate result string
}

static void __uart_tohex(uint32_t x, char *res) {

  static const char symbols[] = "0123456789abcdef";

  int i;
  for (i=0; i<8; i++) { // nibble by nibble
    uint32_t num_tmp = x >> (4*i);
    res[7-i] = (char)symbols[num_tmp & 0x0f];
  }

  res[8] = '\0'; // terminate result string
}

static void __uart_touppercase(uint32_t len, char *ptr) {

  char tmp;

  while (len > 0) {
    tmp = *ptr;
    if ((tmp >= 'a') && (tmp <= 'z')) {
      *ptr = tmp - 32;
    }
    ptr++;
    len--;
  }
}



#define sgetc(buffer) (*((buffer)++))


int vsscanf(const char *string, const char *format, va_list args)
{
    return 0;
}

int sscanf(const char *string, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsscanf(string, format, args);
    va_end(args);
    return result;
}

static size_t __read_until(FILE *stream, char *buffer)
{
    size_t size = 0;
    char c;
    while ((c = fgetc(stream)) != '\t' && c != '\r' && c != '\n' && c != ' ')
        buffer[size++] = c;
    return size;
}

int vfscanf(FILE *stream, const char *format, va_list args)
{
    char buffer[256];
    char f, c;

    while ((f = *format++))
    {
        if (f == '%')
        {
            switch (f)
            {
                case 's':
                    {
                        char *dst = va_arg(args, char*);
                        __read_until(stream, dst);
                        break;
                    }
                case '':
                    buffer[__read_until(stream, buffer, 255)];
                default:
                    break;
            }
        }
        else
        {
            while ((c = fgetc(stream) != f);
        }
    }
}

int fscanf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfscanf(stream, format, args);
    va_end(args);
    return result;
}

int vfprintf(FILE *stream, const char *format, va_list args)
{
    char c, string_buf[12];
    int32_t n;

    while ((c = *format++))
    {
        if (c == '%')
        {
            c = *format++;
            switch (c)
            {

                case 's': // string
                    fputs(va_arg(args, char*), stream);
                    break;

                case 'c': // char
                    fputc((char)va_arg(args, int), stream);
                    break;

                case 'i': // 32-bit signed
                case 'd':
                    n = (int32_t)va_arg(args, int32_t);
                    if (n < 0)
                    {
                        n = -n;
                        fputc('-', stream);
                    }
                    __uart_itoa((uint32_t)n, string_buf);
                    fputs(string_buf, stream);
                    break;

                case 'u': // 32-bit unsigned
                    __uart_itoa(va_arg(args, uint32_t), string_buf);
                    fputs(string_buf, stream);
                    break;

                case 'x': // 32-bit hexadecimal
                case 'p':
                case 'X':
                    __uart_tohex(va_arg(args, uint32_t), string_buf);
                    if (c == 'X')
                    {
                        __uart_touppercase(11, string_buf);
                    }
                    fputs(string_buf, stream);
                    break;

                case '%': // escaped percent sign
                    fputc('%', stream);
                    break;

                default: // unsupported format
                    fputc('%', stream);
                    fputc(c, stream);
                    break;
            }
        }
        else
        {
            if (c == '\n')
            {
                fputc('\r', stream);
            }
            fputc(c, stream);
        }
    }

    return 0;
}

int fprintf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

#define sputc(buffer, c) *(buffer++) = (c)
#define sputs(buffer, str)\
    {\
        char c;\
        char *__str = (str);\
        while ((c = *(__str++)) != '\0') sputc(buffer, c);\
    }

int vsprintf(char *buffer, const char *format, va_list args)
{
    char c, string_buf[12];
    int32_t n;

    while ((c = *format++))
    {
        if (c == '%')
        {
            c = *format++;
            switch (c)
            {

                case 's': // string
                    sputs(buffer, va_arg(args, char*));
                    break;

                case 'c': // char
                    sputc(buffer, (char)va_arg(args, int));
                    break;

                case 'i': // 32-bit signed
                case 'd':
                    n = (int32_t)va_arg(args, int32_t);
                    if (n < 0)
                    {
                        n = -n;
                        sputc(buffer, '-');
                    }
                    __uart_itoa((uint32_t)n, string_buf);
                    sputs(buffer, string_buf);
                    break;

                case 'u': // 32-bit unsigned
                    __uart_itoa(va_arg(args, uint32_t), string_buf);
                    sputs(buffer, string_buf);
                    break;

                case 'x': // 32-bit hexadecimal
                case 'p':
                case 'X':
                    __uart_tohex(va_arg(args, uint32_t), string_buf);
                    if (c == 'X')
                    {
                        __uart_touppercase(11, string_buf);
                    }
                    sputs(buffer, string_buf);
                    break;

                case '%': // escaped percent sign
                    sputc(buffer, '%');
                    break;

                default: // unsupported format
                    sputc(buffer, '%');
                    sputc(buffer, c);
                    break;
            }
        }
        else
        {
            if (c == '\n')
            {
                sputc(buffer, '\r');
            }
            sputc(buffer, c);
        }
    }

    return 0;
}

int sprintf(char *buffer, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vsprintf(buffer, format, args);
    va_end(args);
    return result;
}

char fgetc(FILE *stream)
{
    while ((stream->CTRL & (1<<UART_CTRL_RX_NEMPTY)) != 0);
    return (char)(stream->DATA >> UART_DATA_RTX_LSB);
}

char *fgets(char *str, int size, FILE *stream)
{
    int i;
    for (i = 0; i < size - 1; i++)
    {
        while ((stream->CTRL & (1<<UART_CTRL_RX_NEMPTY)) != 0);
        str[i] = (char)(stream->DATA >> UART_DATA_RTX_LSB);
    }
    str[i] = '\0';
    return str - i;
}

int fputc(int c, FILE *stream)
{
    while ((stream->CTRL & (1<<UART_CTRL_TX_FULL)));
    stream->DATA = (uint32_t)c << UART_DATA_RTX_LSB;
    return c;
}

int fputs(const char *str, FILE *stream)
{
    int i = 0;
    char c;
    while ((c = *str++) != '\0')
    {
        while ((stream->CTRL & (1<<UART_CTRL_TX_FULL)));
        stream->DATA = (uint32_t)c << UART_DATA_RTX_LSB;
        i++;
    }
    return i;
}

size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream)
{
    for (size_t i = 0; i < size * nitems; i++)
    {
        ((char*)ptr)[i] = fgetc(stream);
    }
    return size * nitems;
}

size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream)
{
    for (size_t i = 0; i < size * nitems; i++)
    {
        fputc(((char*)ptr)[i], stream);
    }
    return size * nitems;
}






