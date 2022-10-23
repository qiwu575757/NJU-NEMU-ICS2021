#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	for (int i = 0; i < 32; i ++) {
		bool success = true;
		word_t val = isa_reg_str2val(regs[i],&success );	
		printf("%s\t\t0x",regs[i]);
		
		// add 0x print
		int s[30];
		if (val == 0){
			printf("0");
		}	
		
		int m = 0;
		while (val != 0) {
			int c = val % 16;
			val = val / 16;
			s[m++] = c;
		}

		for (int j = m-1; j >= 0; j --) {
			if (s[j] >= 10) {
				printf("%c", (s[i]+55));
			}
			else {
				printf("%d", s[i]);
			}
		}

		printf("\n");
	}

}

// success is ?
word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
