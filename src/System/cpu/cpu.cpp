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
    kout << " OK" << endl;
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


i32 timer_ticks = 0;
i16 timer_phase = 18;

i32 timer_seconds = 0;

void PIT::init() {
    IRQ::installIRQ(0, System::CPU::PIT::TimerHandler);
    kout << "PIT INIT  OK" << endl;
}

void PIT::TimerPhase(i16 hz) {
    INT_TO_STRING(TimerPhaseStr, hz);
    kout << "TimerPhase Reset to " << TimerPhaseStr << " hz" << endl;

    timer_phase = hz;
    int divisor = 1193180 / hz;             /* Calculate our divisor */
    Port::byte_out(0x43, 0x36);             /* Set our command byte 0x36 */
    Port::byte_out(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    Port::byte_out(0x40, divisor >> 8);     /* Set high byte of divisor */
}


void PIT::TimerHandler(register_t *r) {
    
    timer_ticks++;

    if (timer_ticks % timer_phase == 0) {
        NO_INSTRUCTION;
        timer_seconds++;
    }

    PIC::SendEOI(0);
}

void PIT::Sleep(i16 ms) {
    

    i16 timerTicksNeeded = timer_ticks + (timer_phase * (ms / 1000));
    INT_TO_STRING(timerTstr, timerTicksNeeded);
    INT_TO_STRING(timerNOWstr, timer_ticks);

    

    i32 timerold = timer_ticks;

    int i = 0;
    while (i < 600) {
        NO_INSTRUCTION;

        if (timer_ticks == timerTicksNeeded) {
            return;
        }
        else {
            if (timer_ticks % timer_phase == 0 && timerold != timer_ticks) {
                NO_INSTRUCTION;

                i++;
                timerold = timer_ticks;
            }
        }
    }

    kout << "Sleep Timed out!" << endl;
}

#define CURRENT_YEAR        2021                            // Change this each year!
 
int century_register = 0x00;                                // Set by ACPI table parsing code if possible
 
i16 second;
i16 minute;
i16 hour;
i16 day;
i16 month;
i32 year;
 

void RTC::Read() {
    i16 century;
    i16 last_second;
    i16 last_minute;
    i16 last_hour;
    i16 last_day;
    i16 last_month;
    i16 last_year;
    i16 last_century;
    i16 registerB;

    // Note: This uses the "read registers until you get the same values twice in a row" technique
    //       to avoid getting dodgy/inconsistent values due to RTC updates

    while (RTC::getUpdateFlag());                // Make sure an update isn't in progress
    second = RTC::GetRegister(0x00);
    minute = RTC::GetRegister(0x02);
    hour = RTC::GetRegister(0x04);
    day = RTC::GetRegister(0x07);
    month = RTC::GetRegister(0x08);
    year = RTC::GetRegister(0x09);
    if(century_register != 0) {
        century = RTC::GetRegister(century_register);
    }

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        while (RTC::getUpdateFlag());           // Make sure an update isn't in progress
        second = RTC::GetRegister(0x00);
        minute = RTC::GetRegister(0x02);
        hour = RTC::GetRegister(0x04);
        day = RTC::GetRegister(0x07);
        month = RTC::GetRegister(0x08);
        year = RTC::GetRegister(0x09);
        if(century_register != 0) {
                century = RTC::GetRegister(century_register);
        }
    } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
            (last_day != day) || (last_month != month) || (last_year != year) ||
            (last_century != century) );

    registerB = RTC::GetRegister(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        if(century_register != 0) {
                century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year

    if(century_register != 0) {
        year += century * 100;
    } else {
        year += (CURRENT_YEAR / 100) * 100;
        if(year < CURRENT_YEAR) year += 100;
    }
    kout << "RTC Read()" << endl;

}


i8 RTC::GetRegister(int reg) {
    Port::byte_out(cmos_address, reg);
    return Port::byte_in(cmos_data);
}
int RTC::getUpdateFlag() {
    Port::byte_out(cmos_address, 0x0A);
    return (Port::byte_in(cmos_data) & 0x80);
}


i16 RTC::GetSeconds() {
    return second;
}

i16 RTC::GetMin() {
    return minute;
}

i16 RTC::GetHours() {
    return hour;
}

i16 RTC::GetDays() {
    return day;

}
i16 RTC::GetMonth() {
    return month;
}

i16 RTC::GetYear() {
    return year;
}
