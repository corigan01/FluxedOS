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

#include <lib/core/core.h>
#include "../Serial/serial.hpp"
#include "../../lib/StanderdOperations/Operations.h"
#include "../tty/tty.hpp"
#include <lib/assert/assert.hpp>

namespace System
{
    namespace Display
    {
        

        // text mode output
        namespace Driver {
            
            namespace COLOR {
                enum __VGA__COLORS {
                    BLACK = 0,
                    BLUE,
                    GREEN,
                    CYAN,
                    RED,
                    MAGENTA,
                    BROWN,
                    GREY,
                    DARK_GREY,
                    BRIGHT_BLUE,
                    BRIGHT_GREEN,
                    BRIGHT_CYAN,
                    BRIGHT_RED,
                    BRIGHT_MAGENTA,
                    YELLOW,
                    WHITE,
                };

                u16 ColorVar(u8 ForgroundColor, u8 BackroundColor);
            }
            

            class VGA : public tty {
                public:

                    VGA() {
                        kout << "NO INIT INFO ON VGA!" << endl;
                    }

                    VGA (u32 buffer) {
                        init(buffer);

                    }

                    ~VGA() {
                        kout << "NO DEINIT INFO ON VGA!" << endl;
                    }

                    // sets the forground and backround colors
                    void init(u32 buffer) {
                        kout << "Starting VGA Kernel GXDriver" << endl;
                        kout << "\tbuffer : " << (u32)buffer << endl;
                        kout << "\tScreen : " << "80x25" << endl;
                        kout << endl;

                        internalBuffer = (u16*)buffer;
                        
                        BufferSize = 0;
                        lineNumber = 0;


                        clear_screen();

                        cursor_enable(1, 10);
                    }

                    void BufferSet(u16* buffer);

                    void setcolor(u8 f, u8 b) ;
                    void print_char(char c) ;
                    void print_str(const char* str) ;
                    void printf(const char* str, ...) ; 
                    void clear_screen();
                    void clear_line(u32 linenumber);

                    void cursor_update(u8 cs, u8 ce);
                    void cursor_enable(u8 cursor_start, u8 cursor_end);

                private:
                    u16* internalBuffer;
                    int BufferSize = 0;
                    int lineNumber = 0;

                    u8 FColor = COLOR::WHITE;
                    u8 BColor = COLOR::BLACK; 

                    u16 entry(char ch, u8 f, u8 b);

                    uint16 * VBUF;

                protected:

                    template <typename T>
                    void fmat(const char* fmt, T emit, va_list va) {
                        int isLong = 0;
                        auto fetchValue = [&]() -> uintmax_t {
                            int wasLong = isLong;
                            isLong = 0;
                            switch (wasLong) {
                                case 0:
                                    return va_arg(va, unsigned int);
                                case 1:
                                    return va_arg(va, unsigned long);
                                case 2:
                                    return va_arg(va, unsigned long long);
                            }
                            return -1;
                        };

                        for (; *fmt != '\0'; ++fmt) {
                            if (*fmt != '%') {
                                emit(*fmt);
                                continue;
                            }

                            fmt++;
                            while (*fmt == 'l') {
                                ++isLong, ++fmt;
                            }

                            switch (*fmt) {
                                case 'c': {
                                    char ch = va_arg(va, int);
                                    emit(ch);
                                    break;
                                }
                                case 'p': {
                                    const uint32 v = va_arg(va, unsigned long);
                                    INT_TO_STRING(strname, v);
                                    this->print_str(strname);
                                    isLong = 0;
                                    break;
                                }
                                case 'x': {
                                    const uintmax_t v = fetchValue();
                                    INT_TO_STRING(strname, v);
                                    this->print_str(strname);
                                    break;
                                }
                                case 's': {
                                    const char* s = va_arg(va, const char*);
                                    while (*s != '\0')
                                        emit(*s++);
                                    break;
                                }
                                case 'd': {
                                    const uintmax_t v = fetchValue();
                                    INT_TO_STRING(strname, v);
                                    this->print_str(strname);
                                    break;
                                }
                                case 'e': {
                                    const u16 v = fetchValue();
                                    
                                    u8 Fc = (u8)v;
                                    u8 Bc = (u8)(v << 8);

                                    this->setcolor(Fc, Bc);
                                    break;
                                }
                                default:
                                    emit('%');
                                    emit(*fmt);
                                    break;
                            }
                        }
                    }


            };
            

        }

        

    }
} // namespace System
