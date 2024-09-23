#include "string.h"
#include "klib.h"
#include <stddef.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t i;
  for (i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) { return NULL; }

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 != *s2) {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (int i = 0; i < n; i++) {
    if (s1[i] != s2[i])
      return s1[i] - s2[i];
  }
}

void *memset(void *v, int c, size_t n) {
  while (n--) {
    *(char *)v = c;
  }
}

void *memcpy(void *out, const void *in, size_t n) {
  while (n--) {
    *(char *)out = *(char *)in;
  }
}

int memcmp(const void *s1, const void *s2, size_t n) {
  while (n--) {
    if (*(char *)s1 != *(char *)s2) {
      return *(char *)s1 - *(char *)(s2);
    }
    s1 = (const char *)s1 + 1;
    s2 = (const char *)s2 + 1;
  }
}

#endif
