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
#include <lib/string/string.hpp>
#include <System/Graphics/DisplayServer/ds.hpp>
#include <System/Disk/vDisk.hpp>
#include "BUILD.h"
#include <System/fs/mbt.hpp>
#include <System/fs/ext2.hpp>
#include <System/fs/vfs.hpp>

using namespace System;
using namespace System::IO;
using namespace System::VirtualConsole;
using namespace System::Memory;

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t) bus;
    uint32_t lslot = (uint32_t) slot;
    uint32_t lfunc = (uint32_t) func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t) ((lbus << 16) | (lslot << 11) |
                          (lfunc << 8) | (offset & 0xfc) | ((uint32_t) 0x80000000));

    /* write out the address */
    Port::long_out(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t) ((Port::long_in(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

uint16_t pciCheckVendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF) {
        device = pciConfigReadWord(bus, slot, 0, 2);
        return vendor;
    } else {
        return NULL;
    }
}

const char *ClassCodeName[] = {
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
    //NO_INSTRUCTION;

    kout << "Listing PCI..." << endl;
    Graphics::GXDriver::drawstring("Listing PCI", 10, 330, 0xFF0000);

    /*for (int e = 0; e < 16; e++) {
        for (int i = 0; i < 64; i++) {
            u16 pci = pciCheckVendor(e, i);
            u16 device = pciConfigReadWord(e,i,0,2);

            if (pci != NULL) {
                u16 Class = pciConfigReadWord(e, i, 0, 8);

                u16 SubClass = Class >> 8;
                u8 ClassCode  = (u8)Class;

                INT_TO_STRING(pci_s, pci);
                INT_TO_STRING(device_s, device);
                
                Graphics::GXDriver::drawstring("PCI DEVICE: ", 10, 350 + i * 30, 0xFF0000);

                Graphics::GXDriver::drawstring(ClassCodeName[Class], 270, 350 + i * 30, 0x00FF00);


                kout << "\tVender: " << pci << "\t Device: " << device << "\t Class: " << ClassCodeName[Class] << "\t Subclass: " << SubClass << endl;

            }
        }
    }
    kout << endl;*/

    //for(;;) {};


    // Look for all disks
    Disk::init_all_disks();

    // Get disk 0
    Disk::disk_t master = Disk::get_disk(0);

    // Read the partitions of disk 'master'
    fs::import_disk_partitions(master);

    // Get all the partitions
    auto DiskList = fs::GetAllPartitions();

    // init vfs
    fs::init("/");

    // Look through all the partitions and find the one that is EXT2
    for (int i = 0; i < DiskList->size(); i ++) {
        fs::fs_node_t file_system_node;
        file_system_node.partition = DiskList->getat(i);

        if (file_system_node.partition->PartitionType == fs::EXT2) {
            file_system_node.mount_point = "/";

            fs::add_node(file_system_node);
            break;
        }

    }

    // List all Directories
    auto dirs = fs::ListDirectories("/");

    kout << "Printing Directories!" << endl;
    for (int i = 0; i < dirs.size(); i++) {
        kout << "\t" << dirs[i] << endl;
    }

    kout << endl;


    Graphics::GXDriver::drawstring("Starting Display Server!", 10, 550, 0xFF0000);
    //while(1) {};
    kout << "Initializing Display Server..." << endl;
    Graphics::lux DisplayServer;
    DisplayServer.init();


    lwin BackroundWindow2;
    BackroundWindow2.init();
    BackroundWindow2.set_window_position(10, 500);
    BackroundWindow2.set_window_size(400, 200);
    BackroundWindow2.set_window_title("Backround Window");
    BackroundWindow2.set_window_fullscreen(true);


    DisplayServer.attach_window(&BackroundWindow2);
    BackroundWindow2.construct_pointer();
    BackroundWindow2.fillrect(0xFF63F7 - 0x222222, 0, 0, BackroundWindow2.get_window_width(),
                              BackroundWindow2.get_window_height());
    //BackroundWindow2.drawstring("Title", 10, 100, 0xFF0000);

    // 255 99 247
    //0xFF 63 F7
    // 



    lwin InfomationWindow;
    InfomationWindow.init();
    InfomationWindow.set_window_position(500, 500);
    InfomationWindow.set_window_size(400, 200);
    InfomationWindow.set_window_title("Infomation Window");

    DisplayServer.attach_window(&InfomationWindow);
    InfomationWindow.construct_pointer();
    InfomationWindow.fillrect(0x297A5C, 0, 0, InfomationWindow.get_window_width(),
                              InfomationWindow.get_window_height());
    InfomationWindow.drawstring("FluxedOS", 10, 10, 0xFFFFFF);

    INT_TO_STRING(BUILDb, BUILD);

    InfomationWindow.drawstring("Version: 0.0.1 - ", 10, 30, 0xFFFFFF);
    InfomationWindow.drawstring(BUILDb, 220, 30, 0xFFFFFF);
    InfomationWindow.drawstring("Author: corigan01", 10, 50, 0xFFFFFF);
    InfomationWindow.drawstring("Copyright (C) 2022 FluxedOS", 10, 70, 0xFFFFFF);

    INT_TO_STRING(MemoryLeft, pmm::RequestInitial() / (1024 * 1024));
    INT_TO_STRING(BufferAddress, (u32) DisplayServer.get_framebuffer());
    INT_TO_STRING(ScreenWidth, Graphics::GXDriver::getinfo().width);
    INT_TO_STRING(ScreenHeight, Graphics::GXDriver::getinfo().height);

    InfomationWindow.drawstring("Installed Memory   : ", 10, 100, 0xFFFFFF);
    InfomationWindow.drawstring(MemoryLeft, 260, 100, 0xFFFFFF);
    InfomationWindow.drawstring("Buffer Address     : ", 10, 120, 0xFFFFFF);
    InfomationWindow.drawstring(BufferAddress, 260, 120, 0xFFFFFF);
    InfomationWindow.drawstring("Screen Width       : ", 10, 140, 0xFFFFFF);
    InfomationWindow.drawstring(ScreenWidth, 260, 140, 0xFFFFFF);
    InfomationWindow.drawstring("Screen Height      : ", 10, 160, 0xFFFFFF);
    InfomationWindow.drawstring(ScreenHeight, 260, 160, 0xFFFFFF);
    InfomationWindow.drawstring("CPU PIT RTC KBD VMM PAG FPU VBE", 10, 180, 0xFF0000);


    //window.putpixel(10, 10, 0xFF00FF);

    DisplayServer.draw_windows();
    DisplayServer.flip_buffer();

    Graphics::GXDriver::fillrect(0x246b70, 0, 0, Graphics::GXDriver::getinfo().width, 30);
    Graphics::GXDriver::fillcircle(0x297a5c, 10, 10, 50);
    Graphics::GXDriver::drawstring("FluxedOS!", 10, 10, 0xFFFFFF);


    lwin window;

    // Basic commands
    window.init();
    window.set_window_position(100, 100);
    window.set_window_size(200, 200);
    window.set_window_title("Hello World");

    // This allows us to draw shapes to the window
    window.construct_pointer();

    // Tell the DisplayServer that we would like 
    // to display this window to the screen.
    DisplayServer.attach_window(&window);

    u32 TimeAfter = PIT::GetCurrentClock();

    Memory::PrintMemoryMap();

    // this is a shitty while loop for lol
    for (u32 Iter = 0; Iter < 10; Iter++) {
        u32 TimeBefore = PIT::GetCurrentClock();

        for (char i = 0; i < 60; i++) {
            // Fill the backround white 0x FF   FF   FF
            //                            RED GREEN BLUE
            window.fillrect(0xFFFFFF, 0, 0, window.get_window_width(), window.get_window_height());

            // Draw the red circle
            window.fillcircle(0xFF0000, 40, 40, 40);

            // A little "Hello World"
            window.drawstring("Hello World!", 10, 10, 0x00);

            // Draw the 'a'-'z' chars 10 times
            window.drawstring(kout.ToString(i), 10, 30, 0x00);


            // Draw how many times we have done this entire process
            window.drawstring(kout.ToString(Iter), 10, 50, 0x00);
            window.drawstring(kout.ToString(Iter * 60), 10, 70, 0x00);
            window.drawstring(kout.ToString(PIT::GetCurrentClock()), 10, 90, 0x00);

            window.drawstring(kout.ToString(
                    TimeAfter
            ), 10, 110, 0x00);


            // Now redraw the window
            window.redraw_window();


            //DisplayServer.draw_windows();
            //DisplayServer.flip_buffer();

            /*lwin Test_window;
            Test_window.set_window_position(0, 0);
            Test_window.set_window_size(100, 100);
            Test_window.set_window_title("Test");
            Test_window.construct_pointer();

            Test_window.fillrect(0xFFFFFF, 0, 0, Test_window.get_window_width(), Test_window.get_window_height());

            DisplayServer.attach_window(&Test_window);

            Test_window.redraw_window();*/
        }




        TimeAfter = PIT::GetCurrentClock() - TimeBefore;

    }

    Memory::PrintMemoryMap();

    /*
    kout << endl << endl << endl;
    kout << "Memory Test ------------" << endl;
    auto v1 = kmalloc(10);
    
    KernelTTY.setcolor(COLOR::GREEN, COLOR::BLACK);
    kout << (u32)v1 << endl;
    if (v1 != NULL) {
        KernelTTY.print_str("Memory test passed!!");
        KernelTTY.printf(" Got addr *%d for memory! \n", (u32)v1);
    }
    else {
        KernelTTY.print_str("Memory test failed!!");
    }
    kout << "System Done!" << endl;
    KernelTTY.setcolor(COLOR::WHITE, COLOR::BLACK);
    kfree(v1);*/

    /*K_Vector<int> test_vector;

    for (int i = 0; i < 10; i ++) {
        test_vector.push_back(i);
    }

    test_vector.pop_at(3);

    for (auto i = 0; i < test_vector.size() ; i ++) {
        kout << test_vector[i] << endl;
    }
    */


    kout << "Done!" << endl;





    /*console *dev_console;
    auto KernShell = VirtualConsole::BuiltinShell(KernelTTY, COLOR::GREEN, 0);
    dev_console = &KernShell;

    dev_console->begin();

    
    //kout << str4.c_str() << endl;
    //str4 += " 1 2 3 4";
    //kout << str4.c_str() << endl;
    //str4 = "";
    //kout << str4.c_str() << endl;

    

    while(dev_console->IsAlive()) {
        if (Keyboard::TriggerEvent() || 1) {

            dev_console->HandleKeyCode(Keyboard::GetKeyCode());
            //Keyboard::EventHandled();

        }
        
    }*/



    //for(;;) {};

}