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

#include "TextOutput.hpp" 
 
using namespace System::Display::TextMode;
using namespace System;




void VGA::setcolor(COLOR::__VGA__COLORS f, COLOR::__VGA__COLORS b) {
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

void VGA::print_char(char c) {
    switch (c)
    {
    case '\n':
            if (lineNumber > 24 || true) {
                
                uint16 * VBUF;
                int bufS = 80*80;
                memcpy(VBUF, internalBuffer, bufS);

                int VGA_INT = 0;
                for (int i = 80 ; i < bufS - 80*2 ; i++) {
                    internalBuffer[VGA_INT++] = VBUF[i];
                }

                lineNumber = 24;
                BufferSize = 80*24;

                for (int i = 0; i < 80; i++) {
                    internalBuffer[BufferSize + i] = entry(' ', FColor, BColor);
                }
            }
            else {
                BufferSize = 80 * lineNumber;
                lineNumber++;
                //incLine(); 
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
        

        //break;
    }

    //VGA::CURSOR::UPDATE(BufferSize - (lineNumber * 80), lineNumber + 0);
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


