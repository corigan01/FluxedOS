#include "core/core.h"
#include "CppKern.h"


EXTNC_ int kernal_entry() {
	//first init vga with fore & back colors
    
    KernStart();

    while(1){};
    return 0;
}

