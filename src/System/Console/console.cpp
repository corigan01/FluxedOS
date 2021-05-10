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

#include "console.hpp"
#include <System/Keyboard/keyboard.hpp>

using namespace System;
using namespace System::HID;
using namespace System::Console;

console::console(System::Display::tty * tty) {
    OurTTY = tty;
}
console::~console() {
    ;
}

void console::HandleKeyCode(i8 keycode) {
    this->LastChar = keycode;

    switch (keycode) {
    case Keyboard::Keycode::BACKSPACE_PRESSED:

        if (this->CommandLen > 0) {
            OurTTY->print_char('\e');
            this->CommandLen--;    
            NO_INSTRUCTION;
        }

        break;

    case Keyboard::Keycode::ENTER_PRESSED:
        this->HasFinalUserString = true;
        this->UserString[this->CommandLen++] = '\0';
        OurTTY->print_str("\ncommand!\n");

        kout << "User entered command --> \'" << this->UserString << "\'" << endl;

        break;

    default:
        i8 OutputChar = Keyboard::KeycodeAsciiConverter(keycode);

        if (OutputChar != NULL) { 
            OurTTY->print_char(OutputChar); 
            this->UserString[this->CommandLen++] = OutputChar;
        }
        else NO_INSTRUCTION;

        break;
    }
}
char* console::GetString() {
    if (this->HasFinalUserString) {
        this->HasFinalUserString = false;
        return this->UserString;
    }
    return NULL;
}

void console::ReturnInput() {
    OurTTY->print_str("> ");
}