#include <proc.h>
#include "fs.h"

#define MAX_NR_PROC 4

Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg);
Context *context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

static char args1_hello_fun[] = "process 1";
// static char args2_hello_fun[] = "process 2";
static char *args_menu[] = {"/bin/menu", NULL};
// static char *args_exec_test[] = {"/bin/exec-test", "1000", NULL};

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    // Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    printf("\nHello World from Nanos-lite with arg '%s' for the %dth time!\n", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  pcb[0].cp = context_kload(&pcb[0], hello_fun, args1_hello_fun);
  // pcb[1].cp = context_kload(&pcb[1], hello_fun, args2_hello_fun);
  pcb[1].cp = context_uload(&pcb[1], "/bin/menu", args_menu, NULL);
  // pcb[1].cp = context_uload(&pcb[1], "/bin/exec-test", args_exec_test, NULL);

  switch_boot_pcb();
}

Context* schedule(Context *prev) {
  current->cp = prev;
  
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  
  assert(pcb[0].cp != NULL);
  assert(pcb[1].cp != NULL);
  return current->cp;
}

int execve(const char *pathname, char *const argv[], char *const envp[]) {
  // test find file
  int fd = fs_open(pathname, 0, 0);
  if(fd == -1) {
    assert(0);
    return -1;
  }
  else 
    fs_close(fd);

  pcb[1].cp = context_uload(&pcb[1], pathname, argv, envp);
  switch_boot_pcb();
  yield();

  return 0;
}