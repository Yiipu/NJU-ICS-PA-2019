#include "syscall.h"
#include "common.h"

static inline size_t sys_write(int fd, void * buf, size_t len) {
  switch (fd) {
  case 1:
  case 2:
    for (int i = 0; i < len; i++) {
      _putc(*(char *)(buf + i));
    }
    break;
  default:
    panic("fd io not implemented");
  }
  return len;
}

_Context * do_syscall(_Context * c) {
  uintptr_t a[4];
  a[0] = c->GPR1, a[1] = c->GPR2, a[2] = c->GPR3, a[3] = c->GPR4;

  switch (a[0]) {
  case SYS_yield:
    _yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
    _halt(0);
    break;
  case SYS_write:
    c->GPRx = sys_write(a[1], (void *)(a[2]), a[3]);
    break;
  default:
    panic("sys call %d not implemented!", a[0]);
  }
  return NULL;
}
