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

#define endl "\n\0"

namespace System
{
    namespace Display 
    {
        // General output, this is so you can use any output mode and keep the basic output functions

        #define kout System::Display::SerialLog()
        #define TRACE_INFO {__FUNCTION__, __LINE__, __FILE__}
        
        struct TraceInfo {
            const char* Func;
            u32 line;
            const char* file;
        }
        namespace Check 
        {
            bool DidEndLine();
            void EndLine();
            void StartLine();
        }
        
        
        class SerialLog {
            public:        
            SerialLog() { /*System::IO::Serial::init(System::IO::Serial::COM_1);*/ };
            ~SerialLog() {};
            
            SerialLog &operator<<(const char* v);
            SerialLog &operator<<(const int &v);
            SerialLog &operator<<(const u32 &v);
            SerialLog &operator<<(const u16 &v);
            SerialLog &operator<<(const u8 &v);
            SerialLog &operator<<(const char &v);
            SerialLog &operator<<(bool &v);
            SerialLog &operator<<()

            void printf(const char* str, ...);

            const char* ToHex(u32 v);

            
            const char*    BLACK = "\e[0;30m";
            const char*    RED = "\e[0;31m";
            const char*    GREEN = "\e[0;32m";
            const char*    YELLOW = "\e[0;33m";
            const char*    BLUE = "\e[0;34m";
            const char*    MAGENTA = "\e[0;35m";
            const char*    CYAN = "\e[0;36m";
            const char*    WHITE = "\e[0;37m";
            const char*    RESET = "\e[0m";

            const char*    BOLD = "\e[01m";
            const char*    UNDERLINE = "\e[04m";
            const char*    BLINK = "\e[05m";
            const char*    REVERSE = "\e[07m";
            const char*    HIDDEN = "\e[08m";

            const char*    BOLD_BLACK = "\e[01;30m";
            const char*    BOLD_RED = "\e[01;31m";
            const char*    BOLD_GREEN = "\e[01;32m";
            const char*    BOLD_YELLOW = "\e[01;33m";
            const char*    BOLD_BLUE = "\e[01;34m";
            const char*    BOLD_MAGENTA = "\e[01;35m";
            const char*    BOLD_CYAN = "\e[01;36m";
            const char*    BOLD_WHITE = "\e[01;37m";

            const char*    BLINK_BLACK = "\e[05;30m";
            const char*    BLINK_RED = "\e[05;31m";
            const char*    BLINK_GREEN = "\e[05;32m";
            const char*    BLINK_YELLOW = "\e[05;33m";
            const char*    BLINK_BLUE = "\e[05;34m";
            const char*    BLINK_MAGENTA = "\e[05;35m";
            const char*    BLINK_CYAN = "\e[05;36m";
            const char*    BLINK_WHITE = "\e[05;37m";
            

            private:

            void OutputTraceInfo(const char * v) { /*System::IO::Serial::outString(System::IO::Serial::COM_1, "\e[0;33m");*/ };

            template <typename T>
            void fmat(const char* fmt, T emit, va_list va);

            const char hex_c[17] = "0123456789ABCDEF";
        };
    }
}



class example {
    public:

    example() { System::IO::Serial::init(System::IO::Serial::COM_1); }
    ~example() { };
    example &operator<<(const char* i);
    void test();
    
};
