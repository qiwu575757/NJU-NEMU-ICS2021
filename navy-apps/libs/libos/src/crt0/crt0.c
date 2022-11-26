#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {
  int argc = *((int *)args); // 解析出 argc

  char **pargs = (char **)args + 1;
  char ** argv = pargs; // 解析出 argv

  while (pargs != NULL) { // 跳过所有argv[]
    pargs ++;
  }
  char** envp = pargs + 1;
  
  environ = envp;
  exit(main(argc, argv, envp));

  assert(0);
}