
#include "../lib/Term/Term.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"

void port_byte_out (unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}


//IRQs occur whenever a device wants to interact with the CPU
//the CPU will use two PICs, master and slave to interact with devices
//like keyboard or mouse. once the CPU completes the execution of what the device needs
//it will write 0x20 in command register

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

//Following is an array of function pointers, we use this to handle
//custom IRQ handlers for a particular IRQ

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

//Generally, IRQs 0 to 7 are mapped to IDT entries 8 to 15.
//But we already used those IDT entries and the IDT entry 8 is a Double Fault
//So, if we did not map these IRQs to other empty IDT entries, then everytime
//an IRQ0 occurs we get a Double Fault Exception, which is not what we want.
//So, we need to map these IRQs somewhere else
//To do this, we send commands to PICs in order to map them to other unused IDT entries
//for now let's map these to IDT entries 32 to 47

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

//Let's first remap the interrupt controllers and then install them 
//into the correct IDT entries

void irq_install(){
    print_string("Installing IRQs...", WHITE, BLACK);
    //printf("Installing IRQs...");
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
    print_string("OK", GREEN, BLACK);
    print_new_line();

}

//now we use a irq_handler instead of fault_handler
//because for every IRQ a device might want to communicate with the CPU
//like writing a buffer or putting a character on CPU, 
//once the CPU executes what a device needs, it should tell the device
//that the process is completed. To do this it sends an "End Of Interrupt" or EOI
//command which is "0x20" in hex. Most of the modern CPUs have two PICs
//one is Master and the other is Slave. Master has the EOI at 0x20 and the Slave
//has the EOI at 0xA0

void irq_handler(struct regs *r){

    //blank function pointer
    void (*handler)(struct regs *r);

    //check whether we have a custom handler to run the IRQ and execute it
    handler = irq_routines[r->int_no - 32];

    if(handler){
        handler(r);
    }

    //if the IDT entry is greater than 40, then it is second PIC (IRQ8 to IRQ15)
    //then we need to send EOI to Slave or second PIC
    if(r->int_no >= 40){
        port_byte_out(0xA0, 0x20);
    }

    //in other cases,we do the following
    port_byte_out(0x20, 0x20);

}
