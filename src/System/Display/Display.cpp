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
#include <System/memory/pmm/pmm.hpp>

using namespace System;
using namespace System::IO;
using namespace System::Display::Driver;


void VGA::setcolor(i8 f, i8 b) {
    FColor = f;
    BColor = b;
}

uint16_t VGA::entry(char ch, i8 f, i8 b) {
    i16 ax = 0;
    i8 ah = 0, al = 0;

    ah = b;
    ah <<= 4;
    ah |= f;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void VGA::cursor_enable(i8 cursor_start, i8 cursor_end) {
    Port::byte_out(0x3D4, 0x0A);
	Port::byte_out(0x3D5, (Port::byte_in(0x3D5) & 0xC0) | cursor_start);
 
	Port::byte_out(0x3D4, 0x0B);
	Port::byte_out(0x3D5, (Port::byte_in(0x3D5) & 0xE0) | cursor_end);  
}

void VGA::cursor_update(i8 cs, i8 ce) {
    i16 pos = ce * 80 + cs;

    Port::byte_out(0x3D4, 0x0F);
	Port::byte_out(0x3D5, (uint8_t) (pos & 0xFF));
	Port::byte_out(0x3D4, 0x0E);
	Port::byte_out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void VGA::print_char(char c) {
    switch (c)
    {
    case '\n':
            if (lineNumber > 24 || true) {
                
                uint16 * VBUF; // TODO: Malloc this!
                int bufS = 80*80;
                memcpy(VBUF, internalBuffer, bufS);

                int VGA_INT = 0;
                for (int i = 80 ; i < bufS - 80*2 ; i++) {
                    internalBuffer[VGA_INT++] = VBUF[i];
                }

                memset(VBUF, 0x0, bufS);

                lineNumber = 24;
                BufferSize = 80*24;

                for (int i = 0; i < 80; i++) {
                    internalBuffer[BufferSize + i] = entry(' ', FColor, BColor);
                }
            }
            else {
                BufferSize = 80 * lineNumber;
                lineNumber++;
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
        internalBuffer[BufferSize] = this->entry(c, FColor, BColor);
        BufferSize++;
        

        
    }

    cursor_update(BufferSize - (lineNumber * 80), lineNumber + 0);
}
void VGA::clear_screen() {
    auto reset = [this]() {
        uint16 MaxSize = 80 * 25; // TODO: CHANGE THIS!!
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
void VGA::clear_line(i32 linenumber) {
    for (int i = linenumber * 80; i < 80; i ++) {
        internalBuffer[i] = entry(' ', NULL, NULL);
    }
}

void VGA::print_str(const char* str) {
    for (int i = 0 ; i < strlen(str); i++) {
        this->print_char(str[i]);
    }
}

void VGA::printf(const char* str, ...) {
    va_list va;
    va_start(va, str);
    VGA::fmat(str, [this](int ch) { print_char(ch); }, va);
    va_end(va);
}

i16 COLOR::ColorVar(i8 ForgroundColor, i8 BackroundColor) {
    return (BackroundColor << 8) | ForgroundColor;
}