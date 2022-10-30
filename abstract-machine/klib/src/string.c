#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len = len + 1;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t i;

  for (i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
      dst[i] = src[i];
  for ( ; i < n; i++)
      dst[i] = '\0';

  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;

  for (i = 0 ; src[i] != '\0' ; i++)
      dst[dst_len + i] = src[i];
  dst[dst_len + i] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  int len1 = strlen(s1), len2 = strlen(s2);
  int len = len1 > len2 ? len1 : len2;

  for (int i = 0; i < len; i ++) {
    if (s1[i] != s2[i]) {
      return (s1[i]-s2[i]);
    }
  }

  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  int len1 = strlen(s1), len2 = strlen(s2);
  int len = len1 > len2 ? len1 : len2;

  for (int i = 0; i < n && i < len; i ++) {
    if (s1[i] != s2[i]) {
      return (s1[i]-s2[i]);
    }
  }

  return 0;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *p = s;
  for (int i = 0; i < n; i ++) {
    *p = c;
    p ++;
  }

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char *p1 = dst;
  const unsigned char *p2 = src;
  unsigned char p[n+1];

  for (int i = 0; i < n; i ++) {
    p[i] = *p2;

    p2 ++;
  }

  for (int i = 0; i < n; i ++) {
    *p1 = p[i];

    p1 ++;
  }
  

  return dst;

}

void *memcpy(void *out, const void *in, size_t n) {
  unsigned char *p1 = out;
  const unsigned char *p2 = in;

  for (int i = 0; i < n; i ++) {
    *p1 = *p2;

    p1 ++;
    p2 ++;
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1;
  const unsigned char *p2 = s2;

  for (int i = 0; i < n; i ++) {
    if (*p1 != *p2) {
      return (*p1 - *p2);
    }

    p1 ++;
    p2 ++;
  }

  return 0;
}

#endif
