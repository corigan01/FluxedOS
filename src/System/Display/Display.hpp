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
#include "../../lib/StanderdOperations/Operations.hpp"
#include "../tty/tty.hpp"

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

                i16 ColorVar(i8 ForgroundColor, i8 BackroundColor);
            }
            

            class VGA : public tty{
                public:

                    VGA (void* buffer) {
                        internalBuffer = (i16*)buffer;
                        BufferSize = 0;
                        lineNumber = 0;

                        //clear_screen();

                        print_char('*');
                        print_char('\n');
                    }

                    VGA() {};

                    // sets the forground and backround colors
                    void init(void* buffer) {
                        internalBuffer = (i16*)buffer;
                        BufferSize = 0;
                        lineNumber = 0;

                        //clear_screen();

                        print_char('*');
                        print_char('\n');
                    }

                    void setcolor(i8 f, i8 b);
                    void print_char(char c);
                    void print_str(const char* str);
                    void printf(const char* str, ...);
                    void clear_screen();
                    void clear_line(i32 linenumber);

                private:
                    i16* internalBuffer;
                    int BufferSize = 0;
                    int lineNumber = 0;

                    i8 FColor = COLOR::WHITE;
                    i8 BColor = COLOR::BLACK; 

                    i16 entry(char ch, i8 f, i8 b);

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
                                    const i16 v = fetchValue();
                                    
                                    i8 Fc = (i8)v;
                                    i8 Bc = (i8)(v << 8);

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

        // graphics mode output
        namespace vesa {

            

        }
    }
} // namespace System
