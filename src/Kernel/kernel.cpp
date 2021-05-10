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
#include <boot/boot.h>
#include <System/tty/tty.hpp>
#include <System/cpu/cpu.hpp>
#include <System/kout/kout.hpp>
#include <System/Power/Power.hpp>
#include <System/Clock/PIT/PIT.hpp>
#include <System/Clock/RTC/RTC.hpp>
#include <System/memory/pmm/pmm.hpp>
#include <System/Display/Display.hpp>
#include <System/Console/console.hpp>
#include <System/Keyboard/keyboard.hpp>


using namespace System; 
using namespace System::IO;
using namespace System::HID;
using namespace System::CPU;
using namespace System::Clock;
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

    KernelTTY->setcolor(COLOR::BRIGHT_MAGENTA, COLOR::BLACK);    

    IO_WAIT;
    IDT::init();
    KernelTTY->print_str("IDT ");
    IO_WAIT;
    ISR::init();
    KernelTTY->print_str("ISR ");
    IO_WAIT;
    GDT::init();
    KernelTTY->print_str("GDT ");
    IO_WAIT;
    IRQ::init();
    KernelTTY->print_str("IRQ ");
    IO_WAIT;
    EnableINT();
    for (int i = 0; i < 16; i++) {
        PIC::SendEOI(i);
    }
    KernelTTY->print_str("PIC ");
    IO_WAIT;
    PIT::TimerPhase(1000);
    PIT::init();
    KernelTTY->print_str("PIT ");
    RTC::Read();
    KernelTTY->print_str("RTC ");
    Keyboard::installIRQ();
    KernelTTY->print_str("Keyboard ");

    // Init done message
    kout << "INIT     OK" << endl;



    RTC::Date BootTime = RTC::GetDate();
    


    #define BootupLogoColor COLOR::BLACK
    
    { // Tell the user we started the kernel
        KernelTTY->setcolor(COLOR::DARK_GREY, COLOR::DARK_GREY);
        KernelTTY->print_str(R"(
--------------------------------------------------------------------------------------\n)");
        KernelTTY->setcolor(COLOR::BRIGHT_BLUE, BootupLogoColor);
        KernelTTY->print_str(R"(
                   ______            __ __                 __
                  / __/ /_ ____ __  / //_/__ _______  ___ / /
                 / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
                /_/ /_/\___//_\_\ /_/|_|\__/_/ /_//_/\__/_/ )");
        KernelTTY->setcolor(COLOR::WHITE, BootupLogoColor);
        KernelTTY->print_str("\n                BUILD: ");
        INT_TO_STRING(BuildNumberStr, BUILD);
        INT_TO_STRING(MemoryNumberStr, (mbt->mem_lower + mbt->mem_upper) / 1024);
        KernelTTY->setcolor(COLOR::GREEN, BootupLogoColor);
        KernelTTY->print_str(BuildNumberStr);
        KernelTTY->print_str("               ");
        KernelTTY->print_str(MemoryNumberStr);
        KernelTTY->setcolor(COLOR::WHITE, BootupLogoColor);
        KernelTTY->print_str(" MB Installed!\n                Disp Addr: ");
        INT_TO_STRING(FrameBufferStr, mbt->framebuffer_addr);
        INT_TO_STRING(MultiBootInfoStr, mbt->vbe_mode_info);
        KernelTTY->print_str(FrameBufferStr);
        KernelTTY->print_str("         ");
        KernelTTY->printf("%d/%d/%d - %d:%d:%d \n", BootTime.Month, BootTime.Day, BootTime.Year, BootTime.Hour > 12 ? BootTime.Hour - 12 : BootTime.Hour, BootTime.Minute, BootTime.Second);
        KernelTTY->setcolor(COLOR::DARK_GREY, COLOR::DARK_GREY);
        KernelTTY->print_str(R"(
--------------------------------------------------------------------------------------\n)");
        KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
        KernelTTY->print_str("\n\n\n");
    }

    KernelTTY->setcolor(COLOR::BRIGHT_GREEN, COLOR::BLACK);

    KernelTTY->print_str("> ");

    Console::console sh(KernelTTY);

    while (1) {
        sh.HandleKeyCode(Keyboard::GetKeyCode());

        char * CommandOutput = sh.GetString();
        if (CommandOutput != NULL) {
            sh.ReturnInput();
        }
    }


    /* Kernel Finish */

    PIT::Sleep(1000);
    KernelTTY->print_str("Kernel Eneded");
    for (int i = 0; i < 10; i++) {
        PIT::Sleep(1000);
        KernelTTY->print_str(".");
    }
    KernelTTY->print_str("\nPowerHold!\n");

    
    
    Power::hold();
}