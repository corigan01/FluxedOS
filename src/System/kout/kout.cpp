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

#include "kout.hpp"
#include <lib/core/core.h>
#include <System/Serial/serial.hpp>
#include <System/Serial/serial.hpp>
#include <lib/StanderdOperations/Operations.h>

using namespace System;
using namespace System::Display;


    
//SerialLog::SerialLog() {
    //System::IO::Serial::init(System::IO::Serial::COM_1);

//}


bool Check::DidEndLine() {
    return false;
}

void Check::EndLine() {
    //endline = true;
}

void Check::StartLine() {
    //endline = false;
}

void SerialLog::printf(const char* str, ...) {
    OutputTraceInfo((const char*)str);

    va_list va;
    va_start(va, str);
    fmat(str, [this](int ch) { System::IO::Serial::outChar(System::IO::Serial::COM_1, (char)ch ); }, va);
    va_end(va);
}

const char* SerialLog::ToHex(u32 v) {
    static char buf[9];
    buf[8] = 0;
    for (int i = 7; i >= 0; i--) {
        buf[i] = "0123456789ABCDEF"[v & 0xF];
        v >>= 4;
    }
    return buf;
}

SerialLog &SerialLog::operator<<(const TraceInfo v) {
    kout << "[" << v.Func << ":" << v.line << "]: ";


    return *this;
}

SerialLog &SerialLog::operator<<(const char* v){
                
    OutputTraceInfo((const char*)v);

    System::IO::Serial::outString(System::IO::Serial::COM_1, v );

    return *this;
}

SerialLog &SerialLog::operator<<(const int v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const u32 v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const u16 v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const u8 v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const char v) {
    char v_str[2] = {v, '\0'};

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(bool v) {
    const char* v_str = v ? "TRUE" : "FALSE";

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

template <typename T>
void SerialLog::fmat(const char* fmt, T emit, va_list va) {
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
                System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)strname );
                isLong = 0;
                break;
            }
            case 'x': {
                

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
                System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)strname );
                break;
            }

            default:
                emit('%');
                emit(*fmt);
                break;
        }
    }
}

const char* SerialLog::ToString(u32 value) {
    static char rvalue[10] = {};
    itoa(value, rvalue);
    return rvalue;
}

/*template <class T> 
SerialLog &SerialLog::operator<<(const T &v) {
    
    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v);
    return *this;
}

 {

                // Make the terminal look important :)
                System::IO::Serial::outString(System::IO::Serial::COM_1, "\e[0;33m");

                /*
                if (Check::DidEndLine()  ) {
                    System::IO::Serial::outString(System::IO::Serial::COM_1, "[");
                    System::IO::Serial::outString(System::IO::Serial::COM_1, this->CalledFunc);
                    System::IO::Serial::outString(System::IO::Serial::COM_1, ":");
                    INT_TO_STRING(IntStr, CalledLine);
                    
                    System::IO::Serial::outString(System::IO::Serial::COM_1, IntStr);

                    System::IO::Serial::outString(System::IO::Serial::COM_1, "]\t\t --> ");
                    already_displayed = true;
                    Check::StartLine();
                }
                
                if (strcmp(v, endl) == 0) {
                    Check::EndLine();
                }

            }

*/

/*example &example::operator<<(const char* i) {
    System::IO::Serial::outString(System::IO::Serial::COM_1, i);
    return *this;
}

void whyno() {

}*/
