#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

const char *riscv32_regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; i++) {
    if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) {
      printf("\e[1;36m%3s:\e dut = %#12x | ref = %#12x \n", riscv32_regs[i], cpu.gpr[i]._32, ref_r->gpr[i]._32);
      return false;
    }
  }

  if (ref_r->pc != pc) {
    printf("\e[1;36m pc:\e dut = %#12x | ref = %#12x \n", pc, ref_r->pc);
    return false;
  }

  return true;
}

void isa_difftest_attach() {
}
