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
#include "../../lib/StanderdOperations/Operations.hpp"

constexpr const char* endl = "\n\0";

namespace System
{
    namespace Display 
    {
        // General output, this is so you can use any output mode and keep the basic output functions

        #define kout System::Display::SerialLog()
    
        
        
        namespace Check 
        {
            bool DidEndLine();
            void EndLine();
            void StartLine();
        }
        
        
        class SerialLog {
            public:        
            SerialLog() {};
            
            SerialLog &operator<<(const char* v);
            SerialLog &operator<<(const int &v);
            SerialLog &operator<<(const u32 &v);
            SerialLog &operator<<(const u16 &v);
            SerialLog &operator<<(const u8 &v);
            SerialLog &operator<<(const char &v);
            SerialLog &operator<<(bool &v);

            void printf(const char* str, ...);

            private:

            
            static void OutputTraceInfo(const char * v) { System::IO::Serial::outString(System::IO::Serial::COM_1, "\e[0;33m"); };

            template <typename T>
            void fmat(const char* fmt, T emit, va_list va);

            const char hex_c[17] = "0123456789ABCDEF";
        };
    }
}

