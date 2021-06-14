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

#include "panic.hpp"
#include <System/Display/Display.hpp>
#include <System/tty/tty.hpp>

using namespace System;


Display::tty * m_tty;

extern "C" void stack_dump();


struct StackFrame {
    struct StackFrame* edp;
    i32 eip;
};

extern "C" void dump_hex(char* stack) {
    if (stack != NULL)

    kout << "\n\n==========================\nStack dump\n==========================" << endl;
    //kout << (i32)stack << "\n";

    struct StackFrame *Stack;
    asm ("movl %%ebp,%0" : "=r"(Stack) ::);

    kout << "0x";    
    for (i32 i = 0; Stack && i < 1024; i++) {
        kout.printf("%x", Stack->eip);
        
        Stack = Stack->edp;
    }
    kout << endl;
    
    for (int i = 0; i < 0; i++) {
        i8 number = (i8)stack[i];

        if (i % 20 == 0) {
            kout << endl << "0x";
        }
        else if (i % 4 == 0) {
            kout << "\t" << "0x";
        }

        if (number == 0x9A || number == 0xE8 || number == 0xFF / 2) {
            //kout << ":PUSH:";
        }
        else if (number == 0xC2 || number == 0xC3 || number == 0xCA || number == 0xCD) {
            //kout << ":RETN:";
        }
        //else
            kout.printf("%x", number);

        //kout << (i8)stack[i] << "\t";
        
    }
    
    //kout << "\'" << endl;
    kout << endl;
}

void PANIC::smart_panic(multiboot_info_t* mbt, panic_structure panic) {
    {
    auto VGA_DRIVER = Display::Driver::VGA((void*)mbt->framebuffer_addr);        // tell VGA what addr the framebuffer is at
    m_tty = &VGA_DRIVER;   
    }
    
    m_tty->setcolor(Display::Driver::COLOR::WHITE, Display::Driver::COLOR::BRIGHT_RED);
    
    auto lineDisplay = [](i32 for_len, char* ch) {
        for (i8 i = 0; i < for_len; i++) {
            m_tty->print_str(ch);
        }
    };

    lineDisplay(25, "\n");
    lineDisplay(80, "#");
    m_tty->print_str("\n");

    m_tty->print_str(panic.title);
    
    lineDisplay(80 - (strlen(panic.title) + 20), " ");

    m_tty->print_str("TYPE: ");

    switch (panic.type)
    {
    case PANIC::type::FAULT:
        m_tty->print_str("GENERAL FAULT!");
        break;

    case PANIC::type::UNKNOWN_ERROR:
        m_tty->print_str("UNKNOWN ERROR!");
        break;
    
    default:
        break;
    }

    m_tty->print_str("\n");
    lineDisplay(80, "#");
    m_tty->print_str("\n");


    stack_dump();

    while(1);

}

