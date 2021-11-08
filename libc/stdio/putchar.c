#include <stdio.h>
#include <kernel/sbi.h>

int putchar(int ic) {
	char c = (char) ic;
	sbi_console_putc(c);
	return ic;
}
