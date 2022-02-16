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

#include "Display.hpp" 
#include <System/Port/port.hpp>
#include <lib/core/core.h>
#include <System/memory/pmm/pmm.hpp>

using namespace System;
using namespace System::IO;
using namespace System::Display::Driver;


void VGA::setcolor(u8 f, u8 b) {
    FColor = f;
    BColor = b;
}

uint16_t VGA::entry(char ch, u8 f, u8 b) {
    u16 ax = 0;
    u8 ah = 0, al = 0;

    ah = b;
    ah <<= 4;
    ah |= f;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void VGA::cursor_enable(u8 cursor_start, u8 cursor_end) {
    Port::byte_out(0x3D4, 0x0A);
	Port::byte_out(0x3D5, (Port::byte_in(0x3D5) & 0xC0) | cursor_start);
 
	Port::byte_out(0x3D4, 0x0B);
	Port::byte_out(0x3D5, (Port::byte_in(0x3D5) & 0xE0) | cursor_end);  
}

void VGA::cursor_update(u8 cs, u8 ce) {
    u16 pos = ce * 80 + cs;

    Port::byte_out(0x3D4, 0x0F);
	Port::byte_out(0x3D5, (uint8_t) (pos & 0xFF));
	Port::byte_out(0x3D4, 0x0E);
	Port::byte_out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void VGA::BufferSet(u16* buffer) {
    VBUF = buffer;
    memset(VBUF, NULL, sizeof(u16) * 80 * 25);
    
    //for (int i = 0; i < 25; i++) print_char('\n');
}

void VGA::print_char(char c) {
    if ((u32)internalBuffer <= 0) return;
    
    switch (c)
    {
    case '\n':
            if (lineNumber < 25) BufferSize = 80 * lineNumber++;
            else {
                for (int i = 0; i < (80 * 24); i++) internalBuffer[i] = internalBuffer[i + 80];
                for (int i = 0; i < 80; i++) internalBuffer[80 * 24 + i] = 0;
                BufferSize = 80 * 24;
            }
        break;

    case '\r':
            lineNumber--;
            print_char('\n');
        break;

    case '\t':
            for (int i = 0; i < 5; i ++) {
                print_char(' ');
            }
        break;

    case '\e':
        BufferSize--;
        internalBuffer[BufferSize] = entry(' ', FColor, BColor);
        break;

    default:
        if (BufferSize < (80 * 25)) internalBuffer[BufferSize++] = this->entry(c, FColor, BColor);
        break;
    }

    cursor_update(BufferSize % 80, lineNumber - 1);
}
void VGA::clear_screen() {
    auto reset = [this]() {
        uint16 MaxSize = 80 * 24; // TODO: CHANGE THIS!!
        for(int i = 0; i < MaxSize; i++){
            internalBuffer[i] = entry(NULL, FColor, BColor);
        }
        lineNumber = 0;
        BufferSize = 0;
    };

    reset();

    for (int i = 0; i < BufferSize; i++) {
        for (int e = 0; e < 80; e++) {
            print_char(' ');
        }
    }

    reset();
    print_char('\n');


}
void VGA::clear_line(u32 linenumber) {
    for (int i = linenumber * 80; i < 80; i ++) {
        internalBuffer[i] = entry(' ', NULL, NULL);
    }
}

void VGA::print_str(const char* str) {
    for (int i = 0 ; i < strlen(str); i++) {
        if (0x20 <= str[i] <= 0x7e || str[i] == '\n') {
            this->print_char(str[i]);
        }
        else {
            this->print_char(0xfe);
        }
        
    }
}

void VGA::printf(const char* str, ...) {
    va_list va;
    va_start(va, str);
    VGA::fmat(str, [this](int ch) { print_char(ch); }, va);
    va_end(va);
}

u16 COLOR::ColorVar(u8 ForgroundColor, u8 BackroundColor) {
    return (BackroundColor << 8) | ForgroundColor;
}