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

using namespace System;
using namespace System::IO; 
using namespace System::CPU;

// This file may look simple and thats because its only job is to link the C++ with the C functions so asm can talk with us. 
// I don't want to have irq_install() without its protected namespace!

void System::CPU::IDT::init() {
    kout << "IDT INIT ";
    idt_install();
    kout << " OK" << endl;
}

void System::CPU::GDT::init() {
    kout << "GDT INIT ";
    gdt_install();
    kout << " OK" << endl;
}

void System::CPU::IRQ::init() {
    kout << "IRQ INIT ";
    irq_install();
    kout << " OK" << endl;
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

char *Err[] =
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


void System::CPU::IRQ::installIRQ(int irq, void(*handler)(register_t *r)) {
    INT_TO_STRING(PortStr, irq);
    kout << "Installed new IRQ on port: " << PortStr << "!" << endl;
    irq_install_handler(irq, handler);
}

void System::CPU::IRQ::uninstallIRQ(int irq) {
    INT_TO_STRING(PortStr, irq);
    kout << "Uninstalled old IRQ on port: " << PortStr << "!" << endl;
    irq_uninstall_handler(irq);
}

void System::CPU::ISR::init() {
    kout << "ISR INIT ";
    isr_install();
    register_fault_handler(System::CPU::ISR::Err_hanlder);
    kout << " OK" << endl;
}

void System::CPU::ISR::Err_hanlder(register_t *r) {
    
    
    kout << "\n\nCRITICAL ERROR ENCOUNTERED\n=================================" << endl;

    NO_INSTRUCTION;
    NO_INSTRUCTION;

    HALT;
}

void PIC::SendEOI(i8 irq) {
    if(irq >= 8) {
            Port::byte_out(PIC2_COMMAND,PIC_EOI);
    }

        Port::byte_out(PIC1_COMMAND,PIC_EOI);
}

void PIC::Remap(int offset, int offset2) {
    i8 a1, a2;
 
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

void PIC::SetMask(i8 irq) {
    i16 port;
    i8 value;
 
    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = Port::byte_in(port) | (1 << irq);
    Port::byte_out(port, value);   
}

void PIC::ClearMask(i8 irq) {
    uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = Port::byte_in(port) & ~(1 << irq); 
    Port::byte_out(port, value);       
}

static i16 PIC::GetIRQreg(int ocw3) {
    Port::byte_out(PIC1_CMD, ocw3);
    Port::byte_out(PIC2_CMD, ocw3);
    return (Port::byte_in(PIC2_CMD) << 8) | Port::byte_in(PIC1_CMD);
}

i16 PIC::GetIRR() {
    return PIC::GetIRQreg(PIC_READ_IRR);
}

i16 PIC::GetISR() {
    return PIC::GetIRQreg(PIC_READ_ISR);
}


