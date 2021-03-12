#include "../lib/VGA/cVGA.h"
#include "idt.h"
#include "isr.h"



  void _isr0();
  void _isr1();
  void _isr2();
  void _isr3();
  void _isr4();
  void _isr5();
  void _isr6();
  void _isr7();
  void _isr8();
  void _isr9();
  void _isr10();
  void _isr11();
  void _isr12();
  void _isr13();
  void _isr14();
  void _isr15();
  void _isr16();
  void _isr17();
  void _isr18();
  void _isr19();
  void _isr20();
  void _isr21();
  void _isr22();
  void _isr23();
  void _isr24();
  void _isr25();
  void _isr26();
  void _isr27();
  void _isr28();
  void _isr29();
  void _isr30();
  void _isr31();


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
    "Reserved",
    "Reserved"
};


void fault_handler(struct regs *r)
{
    
    //init_vga(WHITE, BLACK);
    SET_COLOR(__RED, __BLACK);
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
    else {
        PRINT_STR(exception_messages[r->int_no]);
        PRINT_STR("^^");
        PRINT_STR(r->int_no + 10);
        PRINT_STR("^^");
    }
}

