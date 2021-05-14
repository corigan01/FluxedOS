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

#include "kernel.hpp"

extern i32 start;
extern i32 end;

#define IO_WAIT for (int i = 0; i < 100000; i++) {};

class Kernel {
    public:

    Kernel(multiboot_info_t* mbt, i32 magic) {
        this->mbt = mbt;

        kout << "Flux Kernel Started..." << endl;                           // tell the console we started the kernel

        auto VGA_DRIVER = Driver::VGA((void*)mbt->framebuffer_addr);        // tell VGA what addr the framebuffer is at
        KernelTTY = &VGA_DRIVER;                                            // bind the tty to the display driver

    }

    ~Kernel() {
        /* Kernel Finish */
        KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
        PIT::Sleep(1000);
        KernelTTY->print_str("\n\n--------------------------------------------------------------------------------------\nKernel Eneded");
        for (int i = 0; i < 10; i++) {
            PIT::Sleep(1000);
            KernelTTY->print_str(".");
        }
        KernelTTY->print_str("\nPowerHold!\n");

        
        
        Power::hold();

    }

    void boot() {
        KernelTTY->setcolor(COLOR::BRIGHT_MAGENTA, COLOR::BLACK);    

        
        GDT::init(); KernelTTY->print_str("GDT ");
        
        IDT::init(); KernelTTY->print_str("IDT ");
        
        ISR::init(); KernelTTY->print_str("ISR ");
        
        IRQ::init(); KernelTTY->print_str("IRQ ");

        
        EnableINT();
        for (int i = 0; i < 16; i++) { PIC::SendEOI(i); }
        KernelTTY->print_str("PIC ");

        
        PIT::TimerPhase(1000);
        PIT::init();
        KernelTTY->print_str("PIT ");

        RTC::Read(); KernelTTY->print_str("RTC ");

        Keyboard::installIRQ();
        KernelTTY->print_str("Keyboard ");

        pmm::init(mbt);
        KernelTTY->print_str("PMM ");

        
        #define BootupLogoColor COLOR::BLACK
        
        { // Tell the user we started the kernel
            RTC::Date BootTime = RTC::GetDate();
            KernelTTY->setcolor(COLOR::DARK_GREY, COLOR::DARK_GREY);
            KernelTTY->print_str(R"(
=========================)");
            KernelTTY->setcolor(COLOR::BRIGHT_BLUE, BootupLogoColor);
            KernelTTY->print_str(R"(
                    ______            __ __                 __
                   / __/ /_ ____ __  / //_/__ _______  ___ / /
                  / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
                 /_/ /_/\___//_\_\ /_/|_|\__/_/ /_//_/\__/_/ )");
            KernelTTY->setcolor(COLOR::WHITE, BootupLogoColor);
            KernelTTY->printf("\n                BUILD: %e%d              %d%e MB Installed!\n                Disp Addr: %d          ", COLOR::ColorVar(COLOR::GREEN, COLOR::BLACK), BUILD, (pmm::PagesAvailable() * PAGE_SIZE) / (1024 * 1024), COLOR::ColorVar(COLOR::WHITE, COLOR::BLACK), mbt->framebuffer_addr );
            KernelTTY->printf("%d/%d/%d - %d:%d:%d \n", BootTime.Month, BootTime.Day, BootTime.Year, BootTime.Hour > 12 ? BootTime.Hour - 12 : BootTime.Hour, BootTime.Minute, BootTime.Second);
            KernelTTY->setcolor(COLOR::DARK_GREY, COLOR::DARK_GREY);
            KernelTTY->print_str(R"(
=========================)");
            KernelTTY->setcolor(COLOR::WHITE, COLOR::BLACK);
            KernelTTY->print_str("\n\n\n");
        }

        kout << "Boot OK" << endl;

    }



    void run() {
        NO_INSTRUCTION;

        pmm::TestMemory(KernelTTY);
       
    }

    multiboot_info_t* mbt;
    tty* KernelTTY;

};

int kmain(multiboot_info_t* mbt, i32 magic) {
    
    Kernel krnl(mbt, magic);
    krnl.boot();
    krnl.run();

    
}