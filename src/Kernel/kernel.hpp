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

    Kernel(multiboot_info_t* multboot, u32 magic) {
        mbt = (multiboot_info_t*)0xC03FF000;

        kout << "Flux Kernel Started..." << endl;                             // tell the console we started the kernel

        //auto VGA_DRIVER = Driver::VGA((void*)0x8B0000);                     // tell VGA what addr the framebuffer is at
        //KernelTTY = &VGA_DRIVER;                                            // bind the tty to the display driver

        
        //KernelTTY  = &VBE_DRIVER;

        kout << "Found Multiboot Header at *" << (u32)mbt << endl;
        kout << "Found Framebuffer addr at *" << (u32)mbt->framebuffer_addr << endl;

        //System::Graphics::Driver::gxinit((void*)mbt->framebuffer_addr, 1024, 768);        

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

    void init_kernel();
    void system_init();

    multiboot_info_t* mbt;
    tty* KernelTTY;
    //Driver::vbe VBE_DRIVER; 


};