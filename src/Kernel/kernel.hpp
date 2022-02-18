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

#pragma once

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
#include <System/CommandHandler/CommandHandler.hpp>
#include <System/Port/port.hpp>
#include <System/PCI/pci.hpp>
#include <System/Graphics/vbe.hpp>
#include <System/memory/staticalloc/skmalloc.hpp>
#include <System/memory/vmm/vmm.hpp>


using namespace System; 
using namespace System::IO;
using namespace System::HID;
using namespace System::CPU;
using namespace System::Clock;
using namespace System::Memory;
using namespace System::Display;
using namespace System::Display::Driver;



class Kernel {
    public:

    Kernel(multiboot_info_t* multboot, u32 magic, u32 boot_page_dir) {
        mbt = multboot;

        //magic = ((u32)mbt) - (24 _KB);

        kout << "Flux Kernel Started..." << endl;                               // tell the console we started the kernel

        if (mbt->framebuffer_type == 3) {                                       // This is GFX mode
            //System::Graphics::Driver::gxinit((void*)(mbt->framebuffer_addr + 0xC0000000), 1024, 768);        
        }
        else { // This is text mode
            KernelTTY.init(mbt->framebuffer_addr + 0xC0000000);;                              // tell VGA what addr the framebuffer is at
                                                                                // bind the tty to the display driver
        }   

        
        kout << "Initializing CPU" << endl;                                     // tell the console we are initializing the system

        CPU::init(mbt); KernelTTY.print_str("CPU ");
        kout << "\tCPU" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;
        GDT::init(); KernelTTY.print_str("GDT ");
        kout << "\tGDT" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;
        IDT::init(); KernelTTY.print_str("IDT ");
        kout << "\tIDT" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;
        ISR::init(); KernelTTY.print_str("ISR ");
        kout << "\tISR" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;
        IRQ::init(); KernelTTY.print_str("IRQ ");
        kout << "\tIRQ" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;
        EnableINT();
        kout << "\tINT" << kout.BOLD_GREEN << "     OK" << kout.YELLOW << endl;

        kout << "\tPIC ";
        for (int i = 0; i < 32; i++) {
            PIC::SendEOI(i);
            if (i % 11 == 0) {
                kout << ".";
            }
             
        }
        kout << kout.BOLD_GREEN << " OK" << kout.YELLOW << endl << endl;

        KernelTTY.print_str("PIC ");

        kout << "Multiboot info (*" << (u32)mbt << ")" << endl;
        kout << "\tflags                : " << mbt->flags << endl;
        kout << "\tmem_lower            : " << mbt->mem_lower << endl;
        kout << "\tmem_upper            : " << mbt->mem_upper << endl;
        kout << "\tboot_device          : " << mbt->boot_device << endl;
        kout << "\tcmdline              : " << mbt->cmdline << endl;
        kout << "\tmods_count           : " << mbt->mods_count << endl;
        kout << "\tmods_addr            : " << mbt->mods_addr << endl;
        kout << "\tmmap_length          : " << mbt->mmap_length << endl;
        kout << "\tmmap_addr            : " << mbt->mmap_addr << endl;
        kout << "\tdrives_length        : " << mbt->drives_length << endl;
        kout << "\tdrives_addr          : " << mbt->drives_addr << endl;
        kout << "\tconfig_table         : " << mbt->config_table << endl;
        kout << "\tboot_loader_name     : " << mbt->boot_loader_name << endl;
        kout << "\tapm_table            : " << mbt->apm_table << endl;
        kout << "\tvbe_control_info     : " << mbt->vbe_control_info << endl;
        kout << "\tvbe_mode_info        : " << mbt->vbe_mode_info << endl;
        kout << "\tvbe_mode             : " << mbt->vbe_mode << endl;
        kout << "\tvbe_interface_seg    : " << mbt->vbe_interface_seg << endl;
        kout << "\tvbe_interface_off    : " << mbt->vbe_interface_off << endl;
        kout << "\tvbe_interface_len    : " << mbt->vbe_interface_len << endl;
        kout << "\tframebuffer_addr     : 0x" << kout.ToHex(mbt->framebuffer_addr) << endl;
        kout << "\tframebuffer_pitch    : " << mbt->framebuffer_pitch << endl;
        kout << "\tframebuffer_width    : " << mbt->framebuffer_width << " bytes" << endl;
        kout << "\tframebuffer_height   : " << mbt->framebuffer_height << " bytes" << endl;
        kout << "\tframebuffer dims     : " << mbt->framebuffer_width / 4 << "x" << (u32)(mbt->framebuffer_height) << endl;
        kout << "\tframebuffer_bpp      : " << mbt->framebuffer_bpp << endl;
        kout << "\tframebuffer_type     : " << mbt->framebuffer_type << endl;
        kout << endl;

        kout << "FRAMEBUFFER DIR ADDRESS: 0x" << (PAGEDIR_INDEX(mbt->framebuffer_addr)) << endl;
      

        vmm::init(mbt, boot_page_dir);
       
        
        //Page::MapPhysRegion(Page::GetPageDir(), SUPER_USER_MEMORY | PRESENT_FLAG | READ_WRITE_ENABLED, mbt->framebuffer_addr, PAGEDIR_INDEX(mbt->framebuffer_addr), 4 _MB);

        vmm::manual_table_fill(vmm::GetSystemDirectory(), PAGEDIR_INDEX(mbt->framebuffer_addr), mbt->framebuffer_addr, (1024 * 768 * 4), USER | PRESENT | R_W );        

        //Page::PrintPageDir((u32)vmm::GetSystemDirectory().table, (u32)vmm::GetSystemDirectory().vtable);

        System::Graphics::Driver::gxinit((void*)(mbt->framebuffer_addr), 1024, 768);
        

        //Page::paging_init((u32*)magic, 24 _KB, boot_page_dir);



        kout << endl << endl;
        

        



    }

    ~Kernel() {
        /* Kernel Finish */
        KernelTTY.setcolor(COLOR::WHITE, COLOR::BLACK);
        PIT::Sleep(1000);
        KernelTTY.print_str("\n\n--------------------------------------------------------------------------------------\nKernel Eneded");
        for (int i = 0; i < 10; i++) {
            PIT::Sleep(1000);
            KernelTTY.print_str(".");
        }
        KernelTTY.print_str("\nPowerHold!\n");

        
        
        Power::hold();

    }

    void init_kernel();
    void system_init();

    multiboot_info_t* mbt;
    Driver::VGA KernelTTY;
    //Driver::vbe VBE_DRIVER; 


};