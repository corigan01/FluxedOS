//#include "CppKern.h"
#include "../lib/Term/Term.h"
#include "../cpu/cpu.h"
#include "../lib/core/core.h"


int kernal_entry() {
    
    gdt_install();
    idt_install();
    isr_install();
    irq_install();
    

    //enable the interrupts
    __asm__ __volatile__ ("sti");

    print_string("Starting C++ Kernel entry point...", YELLOW, BLACK);
    print_string("If you see this screen for more then a few seconds, your build is broken", YELLOW, BLACK);

    //ThrowISR(1);


   
    

    KernStart();
}

