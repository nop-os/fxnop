#ifndef __FXNOP_TERM_H__
#define __FXNOP_TERM_H__

extern int term_x, term_y;

void term_init(void);

void term_putchar(char chr);
void term_putstr(const char *str, int pad_aln, int pad_len, char pad_chr);
void term_putnum(int num, int base, int upper, int pad_aln, int pad_len, char pad_chr);

void term_printf(const char *format, ...);

#endif
