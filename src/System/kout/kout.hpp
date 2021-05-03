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

constexpr const char* endl = "\n";

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
                
                if (Check::DidEndLine() ) {
                    System::IO::Serial::outString(System::IO::Serial::COM_1, "[");
                    System::IO::Serial::outString(System::IO::Serial::COM_1, this->CalledFile);
                    System::IO::Serial::outString(System::IO::Serial::COM_1, " in ");
                    System::IO::Serial::outString(System::IO::Serial::COM_1, this->CalledFunc);
                    System::IO::Serial::outString(System::IO::Serial::COM_1, "]\t --> ");
                    already_displayed = true;
                    Check::StartLine();
                }
                
                if (strcmp((const char*)v, endl) == 0) {
                    Check::EndLine();
                }

                System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v );
            
                

                
                return *this;
            }

            private:

            char* CalledFunc = "";
            char* CalledFile = "";

            
            bool already_displayed = false;
        };
    }
}

