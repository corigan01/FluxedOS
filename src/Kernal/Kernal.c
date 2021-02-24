//#include "CppKern.h"
#include "../lib/Term/Term.h"
#include "../cpu/cpu.h"


int kernal_entry() {
    
    gdt_install();
    idt_install();
    isr_install();
    irq_install();
    

    //enable the interrupts
    __asm__ __volatile__ ("sti");

    print_string("Starting CPP Kernel entry point...", YELLOW, BLACK);


    KernStart();

    
    print_new_line();
    print_string("System stopped for: ", YELLOW, BLACK);
        
    int i = 1;
    while((i / 50000) < 100){
        i++;

        asm volatile("nop");
        
        print_hold_int( ( (i / 50000)) );
    };

    __asm__ __volatile__("cli");

    print_new_line();
    print_string("Goodbye!", RED, WHITE);
    return 0;
}

