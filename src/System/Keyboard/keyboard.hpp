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

#include "../kout/kout.hpp"
#include "../../lib/core/core.h"

namespace System 
{
    namespace HID // Human Interface Device (HID)
    {
        namespace Keyboard 
        {
            namespace Keycode {
                enum __KEY {
                    NULL_KEY = 0,
                    Q_PRESSED = 0x10,
                    Q_RELEASED = 0x90,
                    W_PRESSED = 0x11,
                    W_RELEASED = 0x91,
                    E_PRESSED = 0x12,
                    E_RELEASED = 0x92,
                    R_PRESSED = 0x13,
                    R_RELEASED = 0x93,
                    T_PRESSED = 0x14,
                    T_RELEASED = 0x94,
                    Z_PRESSED = 0x15,
                    Z_RELEASED = 0x95,
                    U_PRESSED = 0x16,
                    U_RELEASED = 0x96,
                    I_PRESSED = 0x17,
                    I_RELEASED = 0x97,
                    O_PRESSED = 0x18,
                    O_RELEASED = 0x98,
                    P_PRESSED = 0x19,
                    P_RELEASED = 0x99,
                    A_PRESSED = 0x1E,
                    A_RELEASED = 0x9E,
                    S_PRESSED = 0x1F,
                    S_RELEASED = 0x9F,
                    D_PRESSED = 0x20,
                    D_RELEASED = 0xA0,
                    F_PRESSED = 0x21,
                    F_RELEASED = 0xA1,
                    G_PRESSED = 0x22,
                    G_RELEASED = 0xA2,
                    H_PRESSED = 0x23,
                    H_RELEASED = 0xA3,
                    J_PRESSED = 0x24,
                    J_RELEASED = 0xA4,
                    K_PRESSED = 0x25,
                    K_RELEASED = 0xA5,
                    L_PRESSED = 0x26,
                    L_RELEASED = 0xA6,
                    Y_PRESSED = 0x2C,
                    Y_RELEASED = 0xAC,
                    X_PRESSED = 0x2D,
                    X_RELEASED = 0xAD,
                    C_PRESSED = 0x2E,
                    C_RELEASED = 0xAE,
                    V_PRESSED = 0x2F,
                    V_RELEASED = 0xAF,
                    B_PRESSED = 0x30,
                    B_RELEASED = 0xB0,
                    N_PRESSED = 0x31,
                    N_RELEASED = 0xB1,
                    M_PRESSED = 0x32,
                    M_RELEASED = 0xB2,

                    ZERO_PRESSED = 0x29,
                    ONE_PRESSED = 0x2,
                    NINE_PRESSED = 0xA,

                    POINT_PRESSED = 0x34,
                    POINT_RELEASED = 0xB4,

                    SLASH_RELEASED = 0xB5,

                    BACKSPACE_PRESSED = 0xE,
                    BACKSPACE_RELEASED = 0x8E,
                    SPACE_PRESSED = 0x39,
                    SPACE_RELEASED = 0xB9,
                    ENTER_PRESSED = 0x1C,
                    ENTER_RELEASED = 0x9C,

                };
            }

            void installIRQ();
            void IRQ_handler(register_t *r);
            void IRQ12_handler(register_t *r);
        
            char KeycodeAsciiConverter(u8 keycode);

            char GetKeyCode();
            char GetKeyChar();

            
            bool TriggerEvent(); // Gives a true statement once a key is pressed for action triggering
            void EventHandled(); // Sets the TriggerEvent to false 
            


        }
    }
}