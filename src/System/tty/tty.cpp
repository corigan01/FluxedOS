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

#include "tty.hpp"
#include "../kout/kout.hpp"
#include "../Serial/serial.hpp"
#include "../Display/Display.hpp"

using namespace System::IO;
using namespace System::Display;

static i8 ttyNumber = 0;

tty::tty() { 
    ttyNumber++;

    INT_TO_STRING(ttyNumberStr, ttyNumber);

    Serial::init(Serial::COM_1); // inits the class so we know we setup COM_1   
    kout << "New tty" << ttyNumberStr << " Started!" << endl; 
    //this->print_str("+++++ TTY STARTED +++++\n");
}

tty::~tty() {

}

void tty::init(void* buffer) {
    // inits the virtual class def
} 
void tty::print_char(char c) {
    Serial::outChar(Serial::COM_1, c);
}
void tty::print_str(const char * str) {
    Serial::outString(Serial::COM_1, (char*)str);
}
void tty::printf(const char *str, ...) {
    ; // do this later when we have ints
}