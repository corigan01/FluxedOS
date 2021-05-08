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
#include "BUILD.b"
#include "../boot/boot.h"
#include "../System/tty/tty.hpp"
#include "../System/cpu/cpu.hpp"
#include "../System/kout/kout.hpp"
#include "../System/Power/Power.hpp"
#include "../System/memory/pmm/pmm.hpp"
#include "../System/Display/Display.hpp"

using namespace System; 
using namespace System::IO;
using namespace System::CPU;
using namespace System::Memory;
using namespace System::Display;
using namespace System::Display::Driver;


extern i32 start;
extern i32 end;

#define IO_WAIT for (int i = 0; i < 100000; i++) {};

int kmain(multiboot_info_t* mbt, i32 magic) {
    

    kout << "Flux Kernel Started..." << endl;                           // tell the console we started the kernel

    auto VGA_DRIVER = Driver::VGA((void*)mbt->framebuffer_addr);      // tell VGA what addr the framebuffer is at
    tty* KernelTTY = &VGA_DRIVER;    // bind the tty to the display driver
    

    
    
    { // Tell the user we started the kernel
        KernelTTY->setcolor(COLOR::WHITE, COLOR::WHITE);
        KernelTTY->print_str(R"(
--------------------------------------------------------------------------------------\n)");
        KernelTTY->setcolor(COLOR::BRIGHT_BLUE, COLOR::BLACK);
        KernelTTY->print_str(R"(
                   ______            __ __                 __
                  / __/ /_ ____ __  / //_/__ _______  ___ / /
                 / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
                /_/ /_/\___//_\_\ /_/|_|\__/_/ /_//_/\__/_/ )");
        KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
        KernelTTY->print_str("\n                BUILD: ");
        INT_TO_STRING(BuildNumberStr, BUILD);
        INT_TO_STRING(MemoryNumberStr, (mbt->mem_lower + mbt->mem_upper) / 1024);
        KernelTTY->setcolor(COLOR::GREEN, COLOR::BLACK);
        KernelTTY->print_str(BuildNumberStr);
        KernelTTY->print_str("               ");
        KernelTTY->print_str(MemoryNumberStr);
        KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
        KernelTTY->print_str(" MB Installed!\n                Disp Addr: ");
        INT_TO_STRING(FrameBufferStr, mbt->framebuffer_addr);
        INT_TO_STRING(MultiBootInfoStr, mbt->vbe_mode_info);
        KernelTTY->print_str(FrameBufferStr);
        KernelTTY->print_str("         ");
        KernelTTY->print_str((char*)mbt->boot_loader_name);
        KernelTTY->setcolor(COLOR::WHITE, COLOR::WHITE);
        KernelTTY->print_str(R"(
--------------------------------------------------------------------------------------\n)");
        KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
        KernelTTY->print_str("\n\n");
    }

    KernelTTY->setcolor(COLOR::MAGENTA, COLOR::BLACK);
    KernelTTY->print_str("Starting CPU INIT();\n");
    kout << "Starting CPU Init()" << endl;

    IO_WAIT;
    IDT::init();
    KernelTTY->print_str("IDT INIT: OK\n");
    IO_WAIT;
    ISR::init();
    KernelTTY->print_str("ISR INIT: OK\n");
    IO_WAIT;
    GDT::init();
    KernelTTY->print_str("GDT INIT: OK\n");
    IO_WAIT;
    IRQ::init();
    KernelTTY->print_str("IRQ INIT: OK\n");
    IO_WAIT;
    EnableINT();
    PIT::TimerPhase(1000);
    for (int i = 0; i < 16; i++) {
        PIC::SendEOI(i);
    }
    KernelTTY->print_str("PIC INIT: OK\n");
    IO_WAIT;
    PIT::init();
    //PIT::TimerPhase(10);
    KernelTTY->print_str("PIT INIT: OK\n");
    RTC::Read();
    KernelTTY->print_str("RTC INIT: OK\n");

    INT_TO_STRING(SecStr, RTC::GetSeconds());
    INT_TO_STRING(MinStr, RTC::GetMin());
    INT_TO_STRING(HourStr, RTC::GetHours());
    INT_TO_STRING(DayStr, RTC::GetDays());
    INT_TO_STRING(Monthstr, RTC::GetMonth());
    INT_TO_STRING(YearStr, RTC::GetYear());

    kout << "Date: " << Monthstr << "/" << DayStr << "/" << YearStr << " - " << HourStr << ":" << MinStr << ":" << SecStr << endl;
    KernelTTY->print_str("Date: ");
    KernelTTY->print_str(Monthstr);
    KernelTTY->print_str("/");
    KernelTTY->print_str(DayStr);
    KernelTTY->print_str("/");
    KernelTTY->print_str(YearStr);
    KernelTTY->print_str(" - ");
    KernelTTY->print_str(HourStr);
    KernelTTY->print_str(":");
    KernelTTY->print_str(MinStr);
    KernelTTY->print_str(":");
    KernelTTY->print_str(SecStr);
    KernelTTY->print_str("\n");


    kout << "INIT     OK" << endl;

    PIT::Sleep(1000);
    KernelTTY->print_str("Kernel Eneded");
    for (int i = 0; i < 10; i++) {
        PIT::Sleep(1000);
        KernelTTY->print_str(".");
    }
    KernelTTY->print_str("\nPowerHold!\n");
    
    Power::hold();
}