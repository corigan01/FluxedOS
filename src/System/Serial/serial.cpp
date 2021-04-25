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

#include "serial.hpp"
#include "../Port/port.hpp"
#include "../../lib/StanderdOperations/Operations.hpp"

using namespace System::IO;
using namespace System::IO;

typedef i8 SerialType; SerialType st = 0;

void Serial::init(Serial::SerialMode mode) {
    st = mode;

    Port::byte_out(mode + 1, 0x00);
    Port::byte_out(mode + 3, 0x80);
    Port::byte_out(mode + 0, 0x03);
    Port::byte_out(mode + 1, 0x00);
    Port::byte_out(mode + 3, 0x03);
    Port::byte_out(mode + 2, 0xC7);
    Port::byte_out(mode + 4, 0x0B);
}

void Serial::outChar(char c) {
    while (Serial::transmitEmpty() == 0);
    Port::byte_out(st , c);
}
void Serial::outString(char * s) {
    for (int i = 0; i < strlen(s); i++) {
        while (Serial::transmitEmpty() == 0);
        Port::byte_out(st , s[i]);
    }
}

int Serial::inByte() {
    while (received() == 0);
    return Port::byte_in(st);
}
int Serial::received() {
    return Port::byte_in(st + 5) & 1;
}
int Serial::transmitEmpty() {
    return Port::byte_in(st + 5) & 0x20;
}
            