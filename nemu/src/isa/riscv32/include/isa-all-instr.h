#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f)   f(jalr) f(jal) f(auipc) \
                        f(addi) \
                        f(lui) f(lw) f(sw) f(inv) f(nemu_trap)

def_all_EXEC_ID();
