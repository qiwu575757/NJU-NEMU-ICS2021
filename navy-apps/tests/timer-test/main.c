#include "/home/qiwu/Documents/ics2021/navy-apps/libs/libndl/include/NDL.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

int main() {
  NDL_Init(0);
  uint32_t next_msec = NDL_GetTicks();
  uint32_t msec = 0;
  while(1) {
    while(msec < next_msec){
      msec = NDL_GetTicks();
    }
    msec = NDL_GetTicks();
    printf("\nsome setence~~\n");
    next_msec += 5000;
  }
  return 0;
}
