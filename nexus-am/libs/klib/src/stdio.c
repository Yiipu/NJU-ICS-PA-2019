#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    char buf[4096];
    int len = vsprintf(buf, fmt, ap);
    if (n == 0) {
        return len;
    }
    if (len >= n) {
        len = n - 1;
    }
    memcpy(out, buf, len);
    out[len] = '\0';
    return len;
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = printk(fmt, args);
    va_end(args);
    return result;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    return vsnprintf(out, 0, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(out, 0, fmt, args);
    va_end(args);
    return result;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(out, n, fmt, args);
    va_end(args);
    return result;
}

#endif
