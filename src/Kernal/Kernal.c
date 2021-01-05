#include "CppKern.h"

void syshold(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

EXTNC_ int kernal_entry() {
	//first init vga with fore & back colors
    
    KernStart();

    while(1){
        static unsigned long long kern_hold = 0;
        kern_hold++;

        syshold(0x02FFFFFF);

        print_int(kern_hold);

    };
    return 0;
}

