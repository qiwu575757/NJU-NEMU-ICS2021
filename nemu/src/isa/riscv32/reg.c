#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	printf("Reg Info\n");
	for (int i = 0; i < 32; i ++) {
		printf("\e[1;36m%3s:\e[0m %#12x | ", regs[i], cpu.gpr[i]._32);
		printf("\n");
	}

}

word_t isa_reg_str2val(const char *s, bool *success) {
		*success = true;
		if (strcmp(s, regs[0]) == 0) {
				return cpu.gpr[0]._32;
		}		

		for (int i = 1; i < 32; i ++) {
				if (strcmp(s+1, regs[i]) == 1) {
						return cpu.gpr[i]._32;
				}
		}

		*success = false;
		return -1;
}
