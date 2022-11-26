#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case Machine_Software_Interrupt:
      case User_Software_Interrupt:
        if (c->GPR1 == -1){ // 特指-1
          ev.event = EVENT_YIELD;
        }else {
          ev.event = EVENT_SYSCALL;
        }
        c->mepc += 4;
        break;


      default: 
        ev.event = EVENT_ERROR; 
        assert(0);
        break;
    }

    // printf("ev event = %d\n", ev.event);
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ctx = (Context *)((uint8_t *)kstack.end - sizeof(Context));
  ctx->mepc = (uintptr_t)entry;
  ctx->GPRx = (uintptr_t)arg;
  ctx->gpr[0] = 0;
  ctx->mstatus = 0x1800;

  return ctx;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
