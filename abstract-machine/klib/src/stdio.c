#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buffer[10000];
  va_list ap;

  va_start(ap, fmt);
  int len = vsprintf(buffer, fmt, ap);

  // for (int i = 0; i < len; i++) {
  //   putch(buffer[i]);
  // }
  putstr(buffer);

  va_end(ap);

  return len;
}

#define START 0
#define RUN   1
int vsprintf(char *out, const char *fmt, va_list ap) {
  int state = START, num_len, num;
  int i = 0, j = 0;

  char print_c;
  int  print_d;
  char* print_s;

  for ( ; fmt[i] != '\0'; i++) {
    switch (state)
    {
      case START:
        if (fmt[i] != '%')
          out[j++] = fmt[i];
        else 
          state = RUN;
        break;

      case RUN:
        switch (fmt[i])
        {
          case 'c':
            print_c = (char) va_arg(ap, int);
            out[j++] = print_c;
            state = START;
            break;
          case 's':
            print_s = va_arg(ap, char *);
            for (int k = 0; print_s[k] != '\0'; k++) {
              out[j++] = print_s[k];
            }
            state = START;
            break;
          case 'd':
            print_d = va_arg(ap, int);
            num = print_d;
            num_len = 0;

            if (print_d == 0) {
              out[j++] = 0 + '0';
            }
            for (; print_d != 0; print_d /= 10) {
              num_len ++;
            }
            for (int k = 0; k < num_len; k++) {
              out[j+num_len-k-1] = num % 10 + '0';
              num /= 10;
            }
            j += num_len;
            state = START;
            break;
          
          default: // 这里对打印精度、位宽的处理有问题。
            state = RUN;
            break;
        }
        break;
      
      default:
        panic("Not implemented");
        break;
    }
  }
  out[j] = '\0';

  return j;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  int len = vsprintf(out, fmt, ap);
  va_end(ap);

  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
