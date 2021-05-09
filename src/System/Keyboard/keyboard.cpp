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

#include "keyboard.hpp"
#include "../cpu/cpu.hpp"
#include "../Port/port.hpp"
#include "../kout/kout.hpp"

using namespace System;
using namespace System::IO;
using namespace System::HID;
using namespace System::CPU;
using namespace System::Display;

#define MAX_KEYCODE_ARR 15 // I don't know why it hangs over 15, but it does

i8 *KeycodeArr     ;
i8 KeycodePointer   = 0;
i8 UsedKeycode      = 0;

void Keyboard::installIRQ() {    

    IRQ::installIRQ(1, System::HID::Keyboard::IRQ_handler);
    PIC::SendEOI(1);

    kout << "Keyboard Init  OK" << endl;
}

static char* LetterArrayQ = "qwertyuiop"; 
static char* LetterArrayA = "asdfghjkl";
static char* LetterArrayY = "zxcvbnm";
static char* NumberArray  = "123456789";

char Keyboard::KeycodeAsciiConverter(i8 keycode) {
    switch (keycode) {
        case 0x1C:
            return '\n';
        case 0x39:
            return ' ';
        case 0xE0:
            return '\r';
        
        case Keyboard::Keycode::POINT_RELEASED:
            return '.';
        case Keyboard::Keycode::SLASH_RELEASED:
            return '/';
        case Keyboard::Keycode::ZERO_PRESSED:
            return '0';
        
        default:
            if(keycode >= Keyboard::Keycode::ONE_PRESSED && keycode <= Keyboard::Keycode::NINE_PRESSED) {
                return NumberArray[keycode - Keyboard::Keycode::ONE_PRESSED];
            }
                
            if(keycode >= 0x10 && keycode <= 0x1C) {
                return LetterArrayQ[keycode - 0x10];
            }

            else if(keycode >= 0x1E && keycode <= 0x26) {
                return LetterArrayA[keycode - 0x1E];
            } 

            else if(keycode >= 0x2C && keycode <= 0x32) {
                return LetterArrayY[keycode - 0x2C];
            }
    }

	return NULL;
}

void  Keyboard::IRQ_handler(register_t *r) {
    for (int i = 0; i < 100000; i++) {
         if((Port::byte_in(0x64) & 1) != 0) {
             i8 K = Port::byte_in(0x60);

            if (KeycodePointer > MAX_KEYCODE_ARR) {
                KeycodePointer = 0;
            }   
            KeycodeArr[0] = K;
            NO_INSTRUCTION;   

            i8 Output = KeycodeAsciiConverter(K);

            if (Output != NULL) {
               kout << "Got Keycode : ";
               Serial::outChar(Serial::COM_1, Output);
               kout << endl;
            }
        
            
        }    
    }
    
    PIC::SendEOI(1);
}

void Keyboard::IRQ12_handler(register_t *r) {
    KeycodeArr[0] = Keyboard::Keycode::Q_PRESSED;
    kout << "Keyboard IRQ" << endl;
    Port::byte_in(0x60);
    PIC::SendEOI(12);
}

char Keyboard::GetKeyCode() {
    if (UsedKeycode > MAX_KEYCODE_ARR) UsedKeycode = 0;
    i8 OutputKeycode = KeycodeArr[0];
    KeycodeArr[0] = NULL;

    return OutputKeycode;
}

char Keyboard::GetKeyChar() {
    return KeycodeAsciiConverter(Keyboard::GetKeyCode());
}


