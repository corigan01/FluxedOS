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

#include "PIT.hpp" 
#include "../../cpu/cpu.hpp"
#include "../../Port/port.hpp"

using namespace System;
using namespace System::IO;
using namespace System::CPU;
using namespace System::Clock;

i32 timer_ticks = 0;
i16 timer_phase = 18;

i32 timer_seconds = 0;

void PIT::init() {
    IRQ::installIRQ(0, System::Clock::PIT::TimerHandler);
    kout << "PIT INIT  OK" << endl;
}

void PIT::TimerPhase(i16 hz) {
    INT_TO_STRING(TimerPhaseStr, hz);
    kout << "TimerPhase Reset to " << TimerPhaseStr << " hz" << endl;

    timer_phase = hz;
    int divisor = 1193180 / hz;             /* Calculate our divisor */
    Port::byte_out(0x43, 0x36);             /* Set our command byte 0x36 */
    Port::byte_out(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    Port::byte_out(0x40, divisor >> 8);     /* Set high byte of divisor */
}


void PIT::TimerHandler(register_t *r) {
    
    timer_ticks++;

    if (timer_ticks % timer_phase == 0) {
        NO_INSTRUCTION;
        timer_seconds++;
    }

    PIC::SendEOI(0);
}

void PIT::Sleep(i16 ms) {
    

    i16 timerTicksNeeded = timer_ticks + (timer_phase * (ms / 1000));
    INT_TO_STRING(timerTstr, timerTicksNeeded);
    INT_TO_STRING(timerNOWstr, timer_ticks);

    

    i32 timerold = timer_ticks;

    int i = 0;
    while (i < 600) {
        NO_INSTRUCTION;

        if (timer_ticks == timerTicksNeeded) {
            return;
        }
        else {
            if (timer_ticks % timer_phase == 0 && timerold != timer_ticks) {
                NO_INSTRUCTION;

                i++;
                timerold = timer_ticks;
            }
        }
    }

    kout << "Sleep Timed out!" << endl;
}

