/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
 *    
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                              
 *  
 *   
 */

#include "cpu.hpp"
#include "../../CPU/cpu.h"
#include "../kout/kout.hpp"
#include "../Port/port.hpp"
#include <System/panic/panic.hpp>

using namespace System;
using namespace System::IO; 
using namespace System::CPU;

// This file may look simple and thats because its only job is to link the C++ with the C functions so asm can talk with us. 
// I don't want to have irq_install() without its protected namespace!

multiboot_info_t* m_mbt;
bool mb_set = false;

void System::CPU::init(multiboot_info_t* mbt) {
    m_mbt = mbt;
    mb_set = true;
}

void System::CPU::IDT::init() {
    //kout << "IDT INIT ";
    idt_install();
    //kout << " OK" << endl;
}

void System::CPU::GDT::init() {
    //kout << "GDT INIT ";
    gdt_install();
    //kout << " OK" << endl;
}

void System::CPU::IRQ::init() {
    //kout << "IRQ INIT ";
    irq_install();
    //kout << " OK" << endl;
}

/*
IRQ
======

IRQ	        Description
----------------------------------------------------------------------
0       	Programmable Interrupt Timer Interrupt                   +
1       	Keyboard Interrupt                                       +
2       	Cascade (used internally by the two PICs. never raised)  +
3       	COM2 (if enabled)                                        +
4       	COM1 (if enabled)                                        +
5       	LPT2 (if enabled)                                        +
6       	Floppy Disk                                              +
7       	LPT1 / Unreliable "spurious" interrupt (usually)         +
8       	CMOS real-time clock (if enabled)                        +
9       	Free for peripherals / legacy SCSI / NIC                 +
10      	Free for peripherals / SCSI / NIC                        +
11      	Free for peripherals / SCSI / NIC                        +
12      	PS2 Mouse                                                +
13      	FPU / Coprocessor / Inter-processor                      +
14      	Primary ATA Hard Disk                                    +
15      	Secondary ATA Hard Disk                                  +
----------------------------------------------------------------------


*/

const char *Err[] =
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
    "Out of Memory",
    "ASSERT",
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
    "SYSCALL"
};


void System::CPU::IRQ::installIRQ(int irq, void(*handler)(register_t *r)) {
    kout << "Installed new IRQ on port: " << irq << "!" << endl;
    irq_install_handler(irq, handler);
}

void System::CPU::IRQ::uninstallIRQ(int irq) {
    kout << "Uninstalled old IRQ on port: " << irq << "!" << endl;
    irq_uninstall_handler(irq);
}

// ISR is the interrupt service routine.
// This is where we handle the interrupts.
// But we can install custom handlers for each interrupt.

void *ISR_HANDLERS[16] = {
        nullptr, nullptr,nullptr,  nullptr,
        nullptr, nullptr,nullptr,  nullptr,
        nullptr, nullptr,nullptr, nullptr,
        nullptr,nullptr,nullptr, nullptr
};

void System::CPU::ISR::installISR(int irq, void(*handler)(register_t *r)) {
    ISR_HANDLERS[irq] = (void*)handler;
}

void System::CPU::ISR::uninstallISR(int irq) {
    ISR_HANDLERS[irq] = 0;
}

void Err_hanlder(struct regs *r) {
    kout << "\0\0\0\0\n" << endl;
    kout << kout.BOLD << kout.BLINK_RED << "\n\nFAULT HANDLER" << kout.YELLOW;
    kout << "\tHeader Address " << kout.BOLD_GREEN << "0x" << kout.ToHex((u32)r) << kout.YELLOW << "\n========================================" << endl;
    if (r->int_no > 32) {
        kout << "\nINTERRUPT ERROR TYPE MISMATCH! COULD NOT FIND INFO HEADER (regs*->int_no > 32)" << endl;
    } 
    else {
        
        kout << "\tInterrupt Number: " << r->int_no << endl;
        kout << "\tError Code: " << r->err_code << endl;
        kout << "\tDescription: " << kout.BOLD_CYAN << Err[r->int_no] << kout.YELLOW << endl;
        kout << "\tREGISTERS: " << endl;
        kout << "\t\t| REG |    DATA    |" << endl;
        kout << "\t\t|-----|------------|" << endl;
        kout << "\t\t| EAX | 0x" << kout.ToHex(r->eax)    << " | " << endl;
        kout << "\t\t| EBX | 0x" << kout.ToHex(r->ebx)    << " | " << endl;
        kout << "\t\t| ECX | 0x" << kout.ToHex(r->ecx)    << " | " << endl;
        kout << "\t\t| EDX | 0x" << kout.ToHex(r->edx)    << " | " << endl;
        kout << "\t\t| ESI | 0x" << kout.ToHex(r->esi)    << " | " << endl;
        kout << "\t\t| EDI | 0x" << kout.ToHex(r->edi)    << " | " << endl;
        kout << "\t\t| EBP | 0x" << kout.ToHex(r->ebp)    << " | " << endl;
        kout << "\t\t| ESP | 0x" << kout.ToHex(r->esp)    << " | " << endl;
        kout << "\t\t| EIP | 0x" << kout.ToHex(r->eip)    << " | " << endl;
        kout << "\t\t| EFG | 0x" << kout.ToHex(r->eflags) << " | " << endl;
        kout << "\t\t|-----|------------|" << endl;
        
        kout << "\tStack Trace: " << endl;
        kout << "\t\t|    ADDR    |    DATA    |   SYMBOL   " << endl;
        kout << "\t\t|------------|------------|------------" << endl;
        kout << "\t\t|     ...    |     ...    |    ...    " << endl;
        u32 *stack = (u32*)r->esp;
        for (int i = 0; i < 10; i++) {
            kout << (i == 2 ? kout.GREEN : kout.YELLOW) << "\t\t| 0x" << kout.ToHex(u32(stack + i)) << " | 0x" <<
            kout.ToHex(stack[i]) << " | " << (stack[i] > 0x0C0000 ? "(NO SYMBOLS) -- > (CAN NOT FIND FUNCTION CALL)" : "N/A") << endl;
        }
        kout << "\t\t|     ...    |     ...    |    ...    " << endl;
        kout << "\t\t|------------|------------|------------" << endl;


    }

    if (r->int_no == 14) {
        kout << "\tPage Fault Infomation: " << endl;
        kout << "\t\t| U | W | P |     User   |" << endl;
        kout << "\t\t|---|---|---|------------|-----------" << endl;
        kout << "\t\t" << (r->err_code == 0 ? kout.GREEN : kout.YELLOW ) << "| 0 | 0 | 0 | Supervisor | tried to read a non-present page entry" << endl;
        kout << "\t\t" << (r->err_code == 1 ? kout.GREEN : kout.YELLOW ) << "| 0 | 0 | 1 | Supervisor | tried to read a page and caused a protection fault" << endl;
        kout << "\t\t" << (r->err_code == 2 ? kout.GREEN : kout.YELLOW ) << "| 0 | 1 | 0 | Supervisor | tried to write to a non-present page entry" << endl;
        kout << "\t\t" << (r->err_code == 3 ? kout.GREEN : kout.YELLOW ) << "| 0 | 1 | 1 | Supervisor | tried to write a page and caused a protection fault" << endl;
        kout << "\t\t" << (r->err_code == 4 ? kout.GREEN : kout.YELLOW ) << "| 1 | 0 | 0 | User       | tried to read a non-present page entry" << endl;
        kout << "\t\t" << (r->err_code == 5 ? kout.GREEN : kout.YELLOW ) << "| 1 | 0 | 1 | User       | tried to read a page and caused a protection fault"  << endl;
        kout << "\t\t" << (r->err_code == 6 ? kout.GREEN : kout.YELLOW ) << "| 1 | 1 | 0 | User       | tried to write to a non-present page entry" << endl;
        kout << "\t\t" << (r->err_code == 7 ? kout.GREEN : kout.YELLOW ) << "| 1 | 1 | 1 | User       | tried to write a page and caused a protection fault" << endl;
        kout << "\t\t" << kout.YELLOW << "|---|---|---|------------|-----------" << endl;
    }
    
    if (ISR_HANDLERS[r->int_no] == 0) {
        kout << kout.BOLD << kout.BOLD_RED << "\n\nNOT ATTACHED FAULT HANDLER!" << kout.YELLOW << endl;
        kout << "========================================" << endl;
        kout << "ERROR TYPE: " << kout.BOLD_RED << "NON RECOVERABLE" << kout.YELLOW << endl;
        kout << "Disabling CPU Int..." << endl;
        CPU::DisableINT();
        kout << "SYSTEM HALTED!" << endl;
        kout << "======\n+HALT+\n======\n";
        HALT;
    }
    else {
        kout << kout.BOLD << kout.GREEN << "\n\nATTACHED FAULT HANDLER!" << kout.YELLOW << endl;
        kout << "========================================" << endl;

        kout << "Passing to custom handler..." << endl;

        HALT;

        void (*handler)(struct regs *r);

        //check whether we have a custom handler to run the ISR and execute it
        handler = (void (*)(regs*))ISR_HANDLERS[r->int_no];

        if(handler){
            handler(r);
        }

        
    }

    
}

void System::CPU::ISR::init() {
    //kout << "ISR INIT ";
    isr_install();
    register_fault_handler(Err_hanlder);
    //kout << " OK" << endl;
}



void PIC::SendEOI(u8 irq) {
    if(irq >= 8) {
            Port::byte_out(PIC2_COMMAND,PIC_EOI);
    }

        Port::byte_out(PIC1_COMMAND,PIC_EOI);
}

void PIC::Remap(int offset, int offset2) {
    u8 a1, a2;
 
    auto io_wait = []() {
        for (int i = 0; i < 10000; i++) { NO_INSTRUCTION }; 
    };


	a1 = Port::byte_in(PIC1_DATA);                        // save masks
	a2 = Port::byte_in(PIC2_DATA);
 
	Port::byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	Port::byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	Port::byte_out(PIC1_DATA, offset);                 // ICW2: Master PIC vector offset
	io_wait();
	Port::byte_out(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	Port::byte_out(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	Port::byte_out(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	Port::byte_out(PIC1_DATA, ICW4_8086);
	io_wait();
	Port::byte_out(PIC2_DATA, ICW4_8086);
	io_wait();
 
	Port::byte_out(PIC1_DATA, a1);   // restore saved masks.
	Port::byte_out(PIC2_DATA, a2);
}

void PIC::SetMask(u8 irq) {
    u16 port;
    u8 value;
 
    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = Port::byte_in(port) | (1 << irq);
    Port::byte_out(port, value);   
}

void PIC::ClearMask(u8 irq) {
    uint16_t port;
    uint8_t value;
 
    if (irq < 8) {
        port = PIC1_DATA;
    } 
    else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = Port::byte_in(port) & ~(1 << irq); 
    Port::byte_out(port, value);       
}

static u16 PIC::GetIRQreg(int ocw3) {
    Port::byte_out(PIC1_CMD, ocw3);
    Port::byte_out(PIC2_CMD, ocw3);
    return (Port::byte_in(PIC2_CMD) << 8) | Port::byte_in(PIC1_CMD);
}

u16 PIC::GetIRR() {
    return PIC::GetIRQreg(PIC_READ_IRR);
}

u16 PIC::GetISR() {
    return PIC::GetIRQreg(PIC_READ_ISR);
}


