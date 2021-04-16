#include "CppKern.h"

#include "../lib/VGA/cVGA.h"
#include "System/multibootHeader.h"



int kernal_entry(multiboot_info_t* mbt, uint32 magic) {

    

    SET_COLOR(__YELLOW, __BLACK);
    PRINT_STR("Starting C++ Kernel entry point...");
    PRINT_STR("If you see this screen for more then a few seconds, your build is broken");


    KernStart(mbt, magic);
}

