#include "CppKern.h"
#include "../lib/Term/Term.h"
#include "../cpu/cpu.h"


int kernal_entry() {
    gdt_install();
    idt_install();
    isr_install();
    irq_install();
    

    //enable the interrupts
    asm volatile("sti");

    //print_string("t: ", WHITE, BLACK);
    //print_int( 0 / 0 );

    //while(1);

    KernStart();


    
    print_new_line();
    print_string("System stopped for: s", YELLOW, BLACK);
    while(1){
        static int i = 0;
        i++;

        asm volatile("nop");
        
        print_hold_int(i / 55000);
    };
    return 0;
}

