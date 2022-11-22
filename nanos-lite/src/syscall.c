#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <proc.h>
#include <sys/time.h>

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void naive_uload(PCB *pcb, const char *filename);

static int execve(const char *pathname, char *const argv[], char *const envp[]) {
  int fd = fs_open(pathname, 0, 0);
  if(fd == -1) {return -1;}
  else fs_close(fd);

  naive_uload(NULL, pathname);

  return 0;
}

void sys_exit(Context *c){
  int status = (int)c->GPR2;
  if (status == 0) {
    execve("/bin/menu", NULL, NULL);
  } else {
    halt(status);
  }
}

void sys_execve(Context *c){
  const char *fname = (const char *)c->GPR2;
  naive_uload(NULL, fname);
  c->GPRx = 0;
}

void sys_write(Context *c){
  int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_read(Context *c){
  int ret = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_brk(Context *c){
  c->GPRx = 0;
}

void sys_open(Context *c){
  int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_close(Context *c){
  int ret = fs_close(c->GPR2);
  c->GPRx = ret;
}

void sys_lseek(Context *c){
  int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_gettimeofday(Context *c){
  struct timeval *tv = (struct timeval *)c->GPR2;
  __uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_usec = (time % 1000000);
  tv->tv_sec = (time / 1000000);
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield:
      sys_yield(c);
      break;

    case SYS_exit:
      sys_exit(c);
      break;

    case SYS_write:
      sys_write(c);
      break;

    case SYS_brk:
      sys_brk(c);
      break;

    case SYS_open:
      sys_open(c);
      break;

    case SYS_close:
      sys_close(c);
      break;

    case SYS_read:
      sys_read(c);
      break;

    case SYS_lseek:
      sys_lseek(c);
      break;

    case SYS_gettimeofday:
      sys_gettimeofday(c);
      break;

    case SYS_execve:
      sys_execve(c);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
