#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/VGA/VGA.h"
#include "../lib/VirtualConsole/VirtualConsole.h"
#include "../cpu/cpu.h"



int KernStart() {
    asm volatile("sti");
    VGA::INIT_DISPLAY();

    VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
    VGA::kprintf("Fluxed OS ====== BUILD %d\n", BUILD);

    VGA::kprintf("Testing VGA commands\nShould be a new line\n Should be %s = %d \nShould not look broken \'T-\eE-\eS-\eT\'\nShould be broken %t %s %d\nDONE!", "ten", 10);

    VirtualConsole console;

    console.Handle();



    


    VGA::PRINT_STR("\n\n");
    VGA::SET_COLOR(VGA::COLORS::RED, VGA::COLORS::BLACK);
    VGA::PRINT_STR("Kernel has exited!");

    //while(1);
    
    //return 1;
    return 0;
}
