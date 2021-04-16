#include "../lib/VGA/cVGA.h"
#include "idt.h"
#include "isr.h"



extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();


void isr_install()
{
    PRINT_STR("ISR INIT ");
    idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)_isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)_isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)_isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)_isr7, 0x08, 0x8E);

    idt_set_gate(8, (unsigned)_isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)_isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)_isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)_isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)_isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)_isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)_isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)_isr15, 0x08, 0x8E);

    idt_set_gate(16, (unsigned)_isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)_isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)_isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)_isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)_isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)_isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)_isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)_isr23, 0x08, 0x8E);

    idt_set_gate(24, (unsigned)_isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)_isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)_isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)_isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)_isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)_isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);

    SET_COLOR(__GREEN, __BLACK);
    PRINT_STR("OK");
    SET_COLOR(__WHITE, __BLACK);
    PRINT_CHAR('\n');

}


char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Out of Memory", // 19
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

isr_t interrupt_handlers[256];

void fault_handler(struct regs *r)
{
    
    //init_vga(WHITE, BLACK);
    SET_COLOR(__BRIGHT_RED, __BLACK);
    PRINT_STR("FAULT ----");
    if (r->int_no < 32)
    {
        PRINT_STR(exception_messages[r->int_no]);
        PRINT_STR(" Exception. System Halted!\n\n\n");
        PRINT_STR("More INFO : ");
        /*print_int(r->cs);
        print_string(" : ", RED, BLACK);
        print_int(r->ds);
        print_string(" : ", RED, BLACK);
        print_int(r->eax);
        print_string(" : ", RED, BLACK);
        print_int(r->ebx);
        print_string(" : ", RED, BLACK);
        print_int(r->ecx);
        print_string(" : ", RED, BLACK);
        print_int(r->edx);
        print_string(" : ", RED, BLACK);
        print_int(r->edi);
        print_string(" : ", RED, BLACK);
        print_int(r->eip);
        print_string(" : ", RED, BLACK);
        print_int(r->err_code);
        print_string(" : ", RED, BLACK);
        print_int(r->es);
        print_string("\n", RED, BLACK);*/
        
        //utb(0xA0, 0x20);
        //outb(0x20, 0x20);

        // FIXME this needs to not be this sad
        while(1);
    } 
    if(interrupt_handlers[r->int_no] != NULL) {
         isr_t handler = interrupt_handlers[r->int_no];
         handler(&r);
    }
}


void register_interrupt_handler(int num, isr_t handler) {
    PRINT_STR("NEW  IRQ ");
    if(num < 256)
        interrupt_handlers[num] = handler;

    SET_COLOR(__GREEN, __BLACK);
    PRINT_STR("OK\n");
    SET_COLOR(__WHITE, __BLACK);
}
