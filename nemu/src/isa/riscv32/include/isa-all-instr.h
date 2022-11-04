#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f)   f(mret) f(csrrw) f(csrrs) f(ecall) \
                        f(beq)  f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
                        f(add)  f(sub) f(sll) f(slt) f(sltu) f(xor) f(srl) f(sra) f(or) f(and)\
                        f(mul)  f(mulh)f(mulhu) f(div) f(divu) f(rem) f(remu) \
                        f(jalr) f(jal) f(auipc) \
                        f(addi) f(slti) f(sltiu) f(xori) f(ori) f(andi) f(slli) f(srli) f(srai) \
                        f(lb)   f(lh)  f(lw)  f(lbu) f(lhu) \
                        f(sb)   f(sh)  f(sw) \
                        f(lui)  f(inv) f(nemu_trap) 

def_all_EXEC_ID();
