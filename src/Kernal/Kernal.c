#include "CppKern.h"


EXTNC_ int kernal_entry() {
    KernStart();


    
    print_new_line();
    print_string("System stopped for: s", YELLOW);
    while(1){
        static int i = 0;
        i++;

        asm volatile("nop");
        
        print_hold_int(i / 55000);
    };
    return 0;
}

