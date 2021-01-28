#include "../lib/Term/Term.h"
#include "idt.h"


//define a structure for the IDT entry
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel; //this where our kernel segment goes in
    unsigned char always0; //as the name suggests, it is always 0
    unsigned char flags; //these are set using the above table
    unsigned short base_hi;
} __attribute__((packed));

//now define an IDT pointer to load it
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

//Now we declare an IDT of 256 entries but we will only use first 
//32 entries for now and represent them as unhandled interrupt
struct idt_entry idt[256];
//define an IDT pointer
struct idt_ptr idtp;

//we call the following function in boot.asm
extern void load_idt();

//to set an entry in the IDT, we can use the following function
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags){
    /* The interrupt routine's base address */
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

//to install the IDT, we can use the following function
void idt_install(){
    //printf("Initializing IDT...");
    print_string("Installing IDT...", WHITE, BLACK);
    //set the special IDT pointer just like we did in gdt.c
    idtp.limit = (sizeof(struct idt_entry)*256)-1;
    idtp.base = (unsigned int)&idt; //point the base of IDT pointer to our idt_entry's address

    //initialize the IDT with zeros
    //memset(&idt, 0, sizeof(struct idt_entry)*256);

    //tell the processor to point the internal register to the new IDT
    load_idt();
    print_string("OK", GREEN, BLACK);
    print_new_line();

}
