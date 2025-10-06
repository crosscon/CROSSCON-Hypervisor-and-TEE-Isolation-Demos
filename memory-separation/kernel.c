#include "io.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

static char buf[256] = "Hello world!\n";
static int ERR = 0;

/**
 * Base can be 10 or 16. If it's 16 then str can start with 0x
 * strtoul returns 0 if it detects error and sets ERR to non-zero value.
 **/
unsigned long int strtoul(const char* str, char** _, int base);

/** Convert unsigned long int to string. Returns buf */
char* ultostr(unsigned long int num, int base, char* buf, int bufsize);
char* u8tostr(uint8_t num, int base, char* buf, int bufsize);
void print_strings(int n, ...);

void main()
{
    uart_init();
    uintptr_t ptr = (uintptr_t)main;
    print_strings(3, "void main() addr: 0x", ultostr(ptr, 16, buf, sizeof(buf)), "\n");

    while (1) {
        print_strings(1, "\nAccess (hex): ");
        uart_readLine(buf, sizeof(buf));

        ptr = (uintptr_t)strtoul(buf, NULL, 16);
        if (ptr == 0 && ERR != 0) {
            print_strings(1, "Couldn't convert string to number!\n");
        }
        else {
            ultostr((unsigned long int)ptr, 16, buf, sizeof(buf));
            print_strings(3, "Trying to access: 0x", buf, "\n");

            u8tostr(*(uint8_t*)ptr, 16, buf, sizeof(buf));
            print_strings(3, "Value: 0x", buf, "\n");
        }
    }
}

unsigned long int strtoul(const char* str, char** _, int base) {
    if (!(*str)) {
        ERR = 1;
        return 0;
    }
    unsigned long int val = 0;
    ERR = 0;
    if (base == 16 && *str == '0' && *(str + 1) == 'x')
        str += 2;

    char digit;
    while((digit = *str++)) {
        if (digit < '0') {
            ERR = 1;
            return 0;
        }
        if (base == 16 && digit >= 'a')
            digit -= 'a' - 10;
        else if (base == 16 && digit >= 'A')
            digit -= 'A' - 10;
        else
            digit -= '0';
        if (digit > 15) {
            ERR = 1;
            return 0;
        }
        val = val*base + digit;
    }
    return val;
}

char* ultostr(unsigned long int num, int base, char* buf, int bufsize) {
    if (num == 0) {
        if (bufsize > 1) {
            *buf = '0';
            *(buf + 1) = '\0';
        }
        else if (bufsize == 1) {
            *buf = '\0';
        }
        return buf;
    }
    char* iter = buf;
    char* end = (buf + bufsize - 1);
    while (iter < end && num != 0) {
        *iter = (num % base);
        if (*iter >= 10)
            *iter += 'a' - 10;
        else
            *iter += '0';
        num /= base;
        ++iter;
    }
    *iter = '\0';
    end = iter - 1;

    // reverse string
    iter = buf;
    while (iter < end) {
        char tmp = *iter;
        *iter = *end;
        *end = tmp;
        ++iter; --end;
    }
    return buf;
}

char* u8tostr(uint8_t num, int base, char* buf, int bufsize) {
    return ultostr(num, base, buf, bufsize);
}

// Variadic function to print given arguments
void print_strings(int n, ...) {
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++)
        uart_writeText(va_arg(args, char*));
    va_end(args);
}
