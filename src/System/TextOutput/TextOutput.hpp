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

#include "../../lib/core/core.h"
#include "../Serial/serial.hpp"
namespace System
{
    namespace Display
    {
        // General output, this is so you can use any output mode and keep the basic output functions
        
        
        // inits the defalut display output, can be changed multiple times!
        template <class T>
        void init(T mode, System::IO::Serial::SerialDevice device = System::IO::Serial::COM_1) {

        }
        
        void clear_screen();
        void clear_line(i32 lineNumber);
        void new_line();
        
        void kprintf(const char* str, ...);
        void kprint(char* str);


        

        // text mode output
        namespace TextMode {
            
            namespace colors {
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
            }
            

            class VGA {
                public:

                    VGA (void* buffer) {
                        internalBuffer = buffer;
                        BufferSize = 0;
                        lineNumber = 0;

                        clear_screen();
                    }

                    // sets the forground and backround colors
                    void setcolor(colors::__VGA__COLORS f, colors::__VGA__COLORS b);
                    void print_char(char c, void* buffer);
                    void clear_screen();
                    void clear_line(i32 linenumber);

                private:
                    void* internalBuffer;
                    int BufferSize = 0;
                    int lineNumber = 0;

            };
            

        }

        // graphics mode output
        namespace vesa {

            //void PRINT_CHAR(char c, void* framebuffer);

        }
    }
} // namespace System
