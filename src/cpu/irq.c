
#include "../lib/VGA/cVGA.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"

void port_byte_out (unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}



 void _irq0();
 void _irq1();
 void _irq2();
 void _irq3();
 void _irq4();
 void _irq5();
 void _irq6();
 void _irq7();
 void _irq8();
 void _irq9();
 void _irq10();
 void _irq11();
 void _irq12();
 void _irq13();
 void _irq14();
 void _irq15();


void *irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

//following function installs a custom IRQ handler for the given IRQ
void irq_install_handler(int irq, void(*handler)(struct regs *r)){
    irq_routines[irq] =  (void*)handler;
}

//to clear or uninstall a IRQ handler
void irq_uninstall_handler(int irq){
    irq_routines[irq] = 0;
}


void irq_remap(void){
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);
}



void irq_install(){
    PRINT_STR("IRQ INIT ");
    irq_remap();

    //mapping the IRQs to 32-47 IDT entries
    idt_set_gate(32, (unsigned)_irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)_irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)_irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)_irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)_irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)_irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)_irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)_irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)_irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)_irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)_irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)_irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)_irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)_irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)_irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)_irq15, 0x08, 0x8E);

    
    SET_COLOR(__GREEN, __BLACK);
    PRINT_STR("OK");
    SET_COLOR(__WHITE, __BLACK);
    PRINT_CHAR('\n');

}


void irq_handler(struct regs *r){

    //blank function pointer
    void (*handler)(struct regs *r);

    //check whether we have a custom handler to run the IRQ and execute it
    handler = irq_routines[r->int_no - 32];

    if(handler){
        handler(r);
        

    }

    if(r->int_no >= 40){
        port_byte_out(0xA0, 0x20);
    }

    //in other cases,we do the following
    port_byte_out(0x20, 0x20);


}
