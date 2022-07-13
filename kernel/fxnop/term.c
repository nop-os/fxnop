#include <fxnop/term.h>
#include <devs/video.h>
#include <stdarg.h>
#include <string.h>

int term_x = 1;
int term_y = 1;

void term_init(void) {
  // TODO: setup cursor in the future
}

void term_putchar(char chr) {
  if (chr == '\n' || term_x > 126 - 5) {
    term_x = 1;
    term_y += 5;
  }
  
  if (term_y > 55 - 5) {
    int height = term_y - (55 - 5);
    video_scroll(height, video_white);
    
    term_y = 55 - 5;
  }
  
  if (chr < ' ') return;
  
  char buffer[2] = {chr, '\0'};
  video_text(buffer, term_x, term_y, video_black);
  
  term_x += 5;
}

void term_putstr(const char *str, int pad_aln, int pad_len, char pad_chr) {
  int length = strlen(str) + 1;

  if (pad_aln) {
    while (pad_len >= length) {
      term_putchar(pad_chr);
      pad_len--;
    }
  }

  while (*str) {
    term_putchar(*str);
    str++;
  }

  if (!pad_aln) {
    while (pad_len >= length) {
      term_putchar(pad_chr);
      pad_len--;
    }
  }
}

void term_putnum(int num, int base, int upper, int pad_aln, int pad_len, char pad_chr) {
  char buffer[100] = {0};
  int offset = 99;

  const char *digits_upper = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const char *digits_lower = "0123456789abcdefghijklmnopqrstuvwxyz";
  const char *digits = (upper ? digits_upper : digits_lower);

  unsigned int value = (unsigned int)(num);

  if (num < 0 && base == 10) {
    value = (unsigned int)(-num);
  }

  if (num == 0) {
    buffer[--offset] = '0';
  } else {
    while (value) {
      buffer[--offset] = digits[value % base];
      value /= base;
    }
  }

  if (num < 0 && base == 10) {
    buffer[--offset] = '-';
  }

  term_putstr(buffer + offset, pad_aln, pad_len, pad_chr);
}

void term_printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  while (*format) {
    if (*format == '%') {
      format++;

      int pad_aln = 1;
      int pad_len = 0;
      char pad_chr = ' ';

      if (*format == '-') {
        pad_aln = 0;
        format++;
      }

      if (*format == '0') {
        pad_chr = '0';
        format++;
      }

      while (*format >= '0' && *format <= '9') {
        pad_len *= 10;
        pad_len += (*format - '0');
        format++;
      }

      switch (*format) {
        case '%':
          term_putchar('%');
          break;
        case 'c':
          term_putchar((char)(va_arg(args, int)));
          break;
        case 'i': case 'd': case 'u':
          term_putnum(va_arg(args, int), 10, 0, pad_aln, pad_len, pad_chr);
          break;
        case 'x':
          term_putnum(va_arg(args, int), 16, 0, pad_aln, pad_len, pad_chr);
          break;
        case 'p':
          pad_chr = '0';
          pad_len = 8;
        case 'X':
          term_putnum(va_arg(args, int), 16, 1, pad_aln, pad_len, pad_chr);
          break;
        case 'o':
          term_putnum(va_arg(args, int), 8, 0, pad_aln, pad_len, pad_chr);
          break;
        case 'b':
          term_putnum(va_arg(args, int), 2, 0, pad_aln, pad_len, pad_chr);
          break;
        case 's':
          term_putstr(va_arg(args, const char *), pad_aln, pad_len, pad_chr);
          break;
      }
    } else {
      term_putchar(*format);
    }

    format++;
  }
}
