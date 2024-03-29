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
#include <System/memory/kmemory.hpp>
#include <System/memory/paging/page.hpp>
#include <System/panic/panic.hpp>
#include <lib/vector/KernelVector.hpp>
#include <System/FPU/fpu.hpp>
#include <System/Graphics/vbe.hpp>
#include <System/memory/vmm/vmm.hpp>
#include "BUILD.h"


void Kernel::init_kernel() {
        

        KernelTTY.setcolor(COLOR::BRIGHT_MAGENTA, COLOR::BLACK);    
        KernelTTY.print_str("STARTING STAGE 1\n");

        Graphics::GXDriver::drawstring("BOOTING FLUXEDOS!", 10, 10, 0xFFFFFF);


        KernelTTY.setcolor(COLOR::BRIGHT_GREEN, COLOR::BLACK);

        // all of this is done in kernel.hpp now, but we can only display after we set it all up
        Graphics::GXDriver::drawstring("CPU", 10, 30, 0xFF0000);
        
        Graphics::GXDriver::drawstring("GDT", 10, 50, 0xFF0000);
        
        Graphics::GXDriver::drawstring("IDT", 10, 70, 0xFF0000);

        Graphics::GXDriver::drawstring("ISR", 10, 90, 0xFF0000);

        Graphics::GXDriver::drawstring("IRQ", 10, 110, 0xFF0000);

        Graphics::GXDriver::drawstring("PIC", 10, 130, 0xFF0000);

        
        PIT::TimerPhase(1000);
        kout << "Timer setup!" << endl;
        PIT::init();

        
        KernelTTY.print_str("PIT ");


        Graphics::GXDriver::drawstring("PIT", 10, 150, 0xFF0000);

        RTC::Update(); KernelTTY.print_str("RTC ");

        Graphics::GXDriver::drawstring("RTC", 10, 170, 0xFF0000);

        FPU::EnableFPU();

        Graphics::GXDriver::drawstring("FPU", 10, 190, 0xFF0000);

        Keyboard::installIRQ();
        KernelTTY.print_str("Keyboard ");

        Graphics::GXDriver::drawstring("KBD", 10, 210, 0xFF0000);

        
        kout << "Done!" << endl;
        KernelTTY.print_str("PMM ");
        Graphics::GXDriver::drawstring("PMM", 10, 230, 0xFF0000);

        //Page::init();
        //Page::id_map(0x0, 0x1, 0xC000000, PRESENT_FLAG | SUPER_USER_MEMORY | READ_WRITE_ENABLED);
        //Page::switch_page(Page::RootDir());
        //Page::enable_paging();
        KernelTTY.print_str("Paging ");
        Graphics::GXDriver::drawstring("Alloc Kernel RAM", 10, 250, 0xFF0000);

        //init_kmalloc(0xC000001);
        
        /*init_memory(mbt);
        
        Page_Entry * Pages;
        size_t Pages_size = 10;

        for (u32 i = 0; i < Pages_size; i++) {
                kout << "Mapping page: " << i << endl;
                Pages[i] = Memory::map_page({});
        }
        kout << "Giving Memory to kmalloc..." << endl;
        Memory::SetPages(Pages, Pages_size);

        kout << "-" << endl;*/


        


        KernelTTY.print_str("kalloc ");
        Graphics::GXDriver::drawstring("Kalloc", 10, 270, 0xFF0000);

        //KernelTTY.BufferSet((u16*)pmm::ReservePage());

        //map_page({PRESENT_FLAG | SUPER_USER_MEMORY | READ_WRITE_ENABLED});
        //

        kout << "TEXT MODE BOOT LOGO" << endl;
        #define BootUpLogoColor COLOR::BLACK
        
        { // Tell the user we started the kernel
            RTC::time_t BootTime = RTC::now();
            KernelTTY.setcolor(COLOR::WHITE, COLOR::WHITE);
            KernelTTY.print_str(R"(
================================================================================)");
            KernelTTY.setcolor(COLOR::BRIGHT_BLUE, BootUpLogoColor);
            KernelTTY.print_str(R"(
                    ______            __ __                 __
                   / __/ /_ ____ __  / //_/__ _______  ___ / /
                  / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
                 /_/ /_/\___//_\_\ /_/|_|\__/_/ /_//_/\__/_/ )");
            KernelTTY.setcolor(COLOR::WHITE, BootUpLogoColor);
            KernelTTY.printf("\n                BUILD: %e%d              %d%e MB Installed!\n                Disp Addr: %d          ", COLOR::ColorVar(COLOR::GREEN, COLOR::BLACK), BUILD, pmm::RequestInitial() / (1024 * 1024), 0x0F, (u32)mbt->framebuffer_addr );
            KernelTTY.printf("%d/%d/%d - %d:%d:%d \n", BootTime.Month, BootTime.Day, BootTime.Year, BootTime.Hour > 12 ? BootTime.Hour - 12 : BootTime.Hour, BootTime.Minute, BootTime.Second);
            KernelTTY.setcolor(COLOR::WHITE, COLOR::WHITE);
            KernelTTY.print_str(R"(
================================================================================)");
            KernelTTY.setcolor(COLOR::WHITE, COLOR::BLACK);
            KernelTTY.print_str("\n\n\n");
        }

        kout << "Boot OK" << endl;      


        //String something = "idk";
        Graphics::GXDriver::drawstring("BOOT!", 10, 310, 0xFF0000);
}
 