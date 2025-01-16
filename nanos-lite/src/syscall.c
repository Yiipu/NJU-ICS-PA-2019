#include "syscall.h"
#include "common.h"
#include "fs.h"

static int programBrk;

static inline int sys_read(int fd, void * buf, size_t len) {
  return fs_read(fd, buf, len);
}

static inline int sys_write(int fd, const void * buf, size_t len) {
  return fs_write(fd, buf, len);
}

static inline int sys_brk(int addr) {
  programBrk = addr;
  return 0;
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
  case SYS_read:
    c->GPRx = sys_read(a[1], (void *)(a[2]), a[3]);
    break;
  case SYS_brk:
    c->GPRx = sys_brk(a[1]);
    break;
  default:
    panic("sys call %d not implemented!", a[0]);
  }
  return NULL;
}
