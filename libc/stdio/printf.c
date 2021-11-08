// Placeholder printf. To be replaced by musl libc implementation.

#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		} else if (*format == 'u') {
            format++;
            unsigned x = va_arg(parameters, unsigned);
            unsigned cx = x;
            unsigned p = 1;
            char c;
            size_t len = 0;
            do {
                cx /= 10;
                len++;
                p *= 10;
            } while(cx);
            p /= 10;

            //if (x < 0)
            //    if (!print("-", sizeof("-")))
            //        return -1;
            while (p) {
                c = '0' + (x / p) % 10;
                if (!print(&c, sizeof(c)))
                    return -1;
                p /= 10;
            }

            written += len;
        } else if (*format == 'l') {
            format++;
            unsigned long long x = va_arg(parameters, unsigned long long);
            unsigned long long cx = x;
            unsigned long long p = 1;
            char c;
            size_t len = 0;
            do {
                cx /= 10;
                len++;
                p *= 10;
            } while(cx);
            p /= 10;

            //if (x < 0)
            //    if (!print("-", sizeof("-")))
            //        return -1;
            while (p) {
                c = '0' + (x / p) % 10;
                if (!print(&c, sizeof(c)))
                    return -1;
                p /= 10;
            }

            written += len;
        } else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
