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
#include "../Serial/serial.hpp"

using namespace System;
using namespace System::Display;


    
SerialLog::SerialLog(const char * function, const char * file, int line) {
    System::IO::Serial::init(System::IO::Serial::COM_1);


    this->CalledFile = (char*)file;
    this->CalledFunc = (char*)function;
    this->CalledLine = line;

}

bool endline = false;

bool Check::DidEndLine() {
    return endline;
}

void Check::EndLine() {
    endline = true;
}

void Check::StartLine() {
    endline = false;
}

void SerialLog::printf(const char* str, ...) {
    OutputTraceInfo((const char*)str);

    va_list va;
    va_start(va, str);
    fmat(str, [this](int ch) { System::IO::Serial::outChar(System::IO::Serial::COM_1, (char)ch ); }, va);
    va_end(va);
}

SerialLog &SerialLog::operator<<(const char* v){
                
    OutputTraceInfo((const char*)v);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v );

    return *this;
}

SerialLog &SerialLog::operator<<(const int &v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const i32 &v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const i16 &v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const i8 &v) {
    INT_TO_STRING(v_str, v);

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(const char &v) {
    char* v_str = "";
    v_str[0] = v;

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
}

SerialLog &SerialLog::operator<<(bool &v) {
    char* v_str = "FALSE";
    
    if (v) {
        v_str = "TRUE";
    }

    OutputTraceInfo(v_str);

    System::IO::Serial::outString(System::IO::Serial::COM_1, (char*)v_str);

    return *this;
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
        