#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char NUM_CHAR[] = "0123456789ABCDEF";
int vsnprintf(char * out, size_t n, const char * fmt, va_list ap) {
  int len = 0;
  char buf[128];
  int buf_len = 0;
  while (*fmt != '\0' && len < n) {
    switch (*fmt) {
    case '%':
      fmt++;
      switch (*fmt) {
      case 'd': {
        int val = va_arg(ap, int);
        if (val == 0) out[len++] = '0';
        if (val < 0) {
          out[len++] = '-';
          val = 0 - val;
        }
        for (buf_len = 0; val; val /= 10, buf_len++) {
          buf[buf_len] = NUM_CHAR[val % 10];
        }
        for (int i = buf_len - 1; i >= 0; i--) {
          out[len++] = buf[i];
        }
        break;
      }
      case '\n':
        out[len++] = '\n';
        break;
      default:
        out[len++] = *fmt;
        break;
      }
      break;
    default:
      out[len++] = *fmt;
      break;
    }
    fmt++;
  }
  out[len] = '\0';
  return len;
}

int printf(const char * fmt, ...) {
  char buffer[1024];
  va_list arg;
  va_start(arg, fmt);
  int result = vsprintf(buffer, fmt, arg);
  printk(buffer);
  va_end(arg);
  return result;
}

int vsprintf(char * out, const char * fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int sprintf(char * out, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsprintf(out, fmt, args);
  va_end(args);
  return result;
}

int snprintf(char * out, size_t n, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(out, n, fmt, args);
  va_end(args);
  return result;
}

#endif