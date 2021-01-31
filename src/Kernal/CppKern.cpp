#include "CppKern.h"
#include "Kernal.h"
#include "BUILD.h"
#include "../lib/Vector/vector.h"
#include "../lib/console/console.h"
#include "../lib/VGA/VGA.h"




int KernStart() {
    asm volatile("sti");
    VGA::INIT_DISPLAY();

    VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
    VGA::PRINT_STR("Hello from FluxedOS ------\n\n\n\ttest\n");

    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
    VGA::PRINT_STR("The big test\n\n");

    VGA::kprintf("test %s test3", "test2");



    while (1);
    
    //return 1;
    return 0;
}
