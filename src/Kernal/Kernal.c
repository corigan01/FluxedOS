//#include "CppKern.h"
#include "../lib/Term/Term.h"
#include "../cpu/cpu.h"


int kernal_entry() {
    init_vga(WHITE, BLACK);
    gdt_install();
    idt_install();
    isr_install();
    irq_install();
    

    //enable the interrupts
    asm ("sti");



    KernStart();


    
    print_new_line();
    print_string("System stopped for: ", YELLOW, BLACK);
    while(1){
        static int i = 0;
        i++;

        asm volatile("nop");
        
        print_hold_int(i / 55000);
    };
    return 0;
}

