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

        #define kout System::Display::SerialLog(__FUNCTION__, __FILE__, __LINE__)
    
        
        
        namespace Check 
        {
            bool DidEndLine();
            void EndLine();
            void StartLine();
        }
        
        
        class SerialLog {
            public:        
            SerialLog(const char * function, const char * file, int line);

            

            template <class T> 
            SerialLog &operator<<(const T &v){
                
                OutputTraceInfo((const char*)v);

                System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v );
            
                return *this;
            }

            void printf(const char* str, ...) {
                OutputTraceInfo((const char*)str);

                va_list va;
                va_start(va, str);
                fmat(str, [this](int ch) { System::IO::Serial::outChar(System::IO::Serial::COM_1, (char)ch ); }, va);
                va_end(va);
            }

            

            private:

            
            void OutputTraceInfo(const char * v) {

                if (Check::DidEndLine() ) {
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
                            System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)strname );
                            isLong = 0;
                            break;
                        }
                        case 'x': {
                            const uintmax_t v = fetchValue();
                            INT_TO_STRING(strname, v);
                            System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)strname );
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

            char* CalledFunc = "";
            char* CalledFile = "";
            int CalledLine = 0;

            
            bool already_displayed = false;
        };
    }
}

