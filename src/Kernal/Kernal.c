#include "CppKern.h"



EXTNC_ int kernal_entry() {
    KernStart();



    
    print_new_line();
    print_string("System stopped for: ", YELLOW);
    while(1){
        static int i = 0;
        i++;

        
        print_hold_int(i / 100);
    };
    return 0;
}

