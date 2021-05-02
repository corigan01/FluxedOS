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
#include "../boot/boot.h"
#include "../System/tty/tty.hpp"
#include "../System/cpu/cpu.hpp"
#include "../System/kout/kout.hpp"
#include "../System/Power/Power.hpp"
#include "../System/Display/Display.hpp"

using namespace System; 
using namespace System::IO;
using namespace System::CPU;
using namespace System::Display;

int kmain(multiboot_info_t* mbt, i32 magic) {
    

    kout << "Flux Kernel Started..." << endl;                           // tell the console we started the kernel

    auto VGA_DRIVER = Driver::VGA((void*)mbt->framebuffer_addr);      // tell VGA what addr the framebuffer is at
    tty* KernelTTY = &VGA_DRIVER;                                       // bind the tty to the display driver
    
    KernelTTY->print_str("Kernel Started!\n");                          // Tell the user we started the kernel
    KernelTTY->print_str(R"(
                   ______            __ __                 __
                  / __/ /_ ____ __  / //_/__ _______  ___ / /
                 / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) / 
                /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/  
                
--------------------------------------------------------------------------------------
)");

    KernelTTY->print_str("Starting CPU INIT();\n");
    kout << "Starting CPU Init()" << endl;

    IDT::init();
    KernelTTY->print_str("IDT INIT: OK\n");
    ISR::init();
    KernelTTY->print_str("ISR INIT: OK\n");
    GDT::init();
    KernelTTY->print_str("GDT INIT: OK\n");
    IRQ::init();
    KernelTTY->print_str("IRQ INIT: OK\n");
    kout << "CPU Init: OK!" << endl;

    
    

    Power::hold();
}