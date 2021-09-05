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
#include <System/Console/console.hpp>
#include <System/CommandHandler/CommandHandler.hpp>
#include <System/memory/kmemory.hpp>

using namespace System;
using namespace System::IO;
using namespace System::VirtualConsole;
using namespace System::Memory;

uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    Port::long_out(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((Port::long_in(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus,slot,0,0)) != 0xFFFF) {
       device = pciConfigReadWord(bus,slot,0,2);
       return vendor;
    } 
    else {
        return NULL;
    }
}

const char * ClassCodeName[] = {
    "Unclassified Device",
    "Mass Storage Controller",
    "Network Controller ",
    "Display Controller ",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station",
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non-Essential Instrumentation",
    "0x3F (Reserved)"
};

void Kernel::system_init() {
    NO_INSTRUCTION;

    //pmm::TestMemory(KernelTTY);
    
    for (int i = 0; i < 64; i++) {
        i16 pci = pciCheckVendor(0, i);
        i16 device = pciConfigReadWord(0,i,0,2);

        if (pci != NULL) {
            i16 Class = pciConfigReadWord(0, i, 0, 8);

            i16 SubClass = Class >> 8;
            i8 ClassCode  = (i8)Class;

            kout << "Device (" << pci << ", " << device << ")\t, Class = " << ClassCodeName[Class] << " : Subclass = " << SubClass << endl;
        }
    }


    kout << endl << endl << endl;
    kout << "Memory Test ------------" << endl;
    auto v1 = kmalloc(10);
    
    kout << (i32)v1 << endl;

    kout << "System Done!" << endl;



    console *dev_console;
    auto KernShell = VirtualConsole::BuiltinShell(KernelTTY, COLOR::GREEN, 0);
    dev_console = &KernShell;

    dev_console->begin();

    while(dev_console->IsAlive()) {
        if (Keyboard::TriggerEvent() || 1) {

            dev_console->HandleKeyCode(Keyboard::GetKeyCode());
            //Keyboard::EventHandled();

        }
        
    }

   
    
        
}