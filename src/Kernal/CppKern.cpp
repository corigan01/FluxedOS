#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/VGA/VGA.h"
#include "../lib/VirtualConsole/VirtualConsole.h"




int KernStart() {
    asm volatile("sti");
    VGA::INIT_DISPLAY();

    VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
    VGA::kprintf("Fluxed OS ====== BUILD %d\n", BUILD);


    VirtualConsole console;

    console.Handle();










    VGA::PRINT_STR("\n\n");
    VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
    VGA::PRINT_STR("Kernel has exited!");

    //while(1);
    
    //return 1;
    return 0;
}
