#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void* old_pf = pf;
  pf = (void *)((char *)pf + nr_page * PGSIZE);

  return old_pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n%PGSIZE == 0);// 分配字节空间为 pagesize的整数倍
  void* p = new_page(n / PGSIZE);
  return memset(p, 0, n);
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
