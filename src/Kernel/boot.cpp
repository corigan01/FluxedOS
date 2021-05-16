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

void Kernel::boot() {
    KernelTTY->setcolor(COLOR::BRIGHT_MAGENTA, COLOR::BLACK);    

    // Setup CPU
    GDT::init(); KernelTTY->print_str("GDT ");
    IDT::init(); KernelTTY->print_str("IDT ");
    ISR::init(); KernelTTY->print_str("ISR ");
    IRQ::init(); KernelTTY->print_str("IRQ ");

    // Enable Int
    EnableINT();
    for (int i = 0; i < 16; i++) { PIC::SendEOI(i); }
    KernelTTY->print_str("PIC ");

    // Set the timer
    PIT::TimerPhase(1000);
    PIT::init();
    KernelTTY->print_str("PIT ");

    // Set the time
    RTC::Read(); 
    KernelTTY->print_str("RTC ");

    // Install Keyboard Handler
    Keyboard::installIRQ();
    KernelTTY->print_str("Keyboard ");

    // Setup Memory
    pmm::init(mbt);
    KernelTTY->print_str("PMM ");

    // Print the output
    #define BootupLogoColor COLOR::BLACK
    { 
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