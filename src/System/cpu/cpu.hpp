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

#include <lib/core/core.h>
#include <System/kout/kout.hpp>
#include <lib/multiboot/multiboot.h>

#define PIC1_CMD                    0x20
#define PIC2_CMD                    0xA0
#define PIC_READ_IRR                0x0a        /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b        /* OCW3 irq service next CMD read */

#define ICW1_ICW4	                0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	                0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	            0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	                0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	                0x10		/* Initialization - required! */
 
#define ICW4_8086	                0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	                0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	            0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	            0x0C		/* Buffered mode/master */
#define ICW4_SFNM	                0x10		/* Special fully nested (not) */

#define PIC1		                0x20		/* IO base address for master PIC */
#define PIC2		                0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	            PIC1
#define PIC1_DATA	                (PIC1 + 1)
#define PIC2_COMMAND	            PIC2
#define PIC2_DATA	                (PIC2 + 1)

#define PIC_EOI		                0x20		/* End-of-interrupt command code */


namespace System 
{
    namespace CPU {

        inline void EnableINT()  {Vasm("sti");};
        inline void DisableINT() {Vasm("cli");};

        void init(multiboot_info_t* mbt);
        namespace IDT {
            void init();
        }

        namespace GDT {
            void init();
        }

        namespace IRQ {
            void init();

            void installIRQ(int irq, void(*handler)(register_t *r));
            void uninstallIRQ(int irq);
        }

        namespace ISR  {
            void init();

            void installISR(int irq, void(*handler)(register_t *r));
            void uninstallISR(int irq);
        }

        namespace PIC {

            void SendEOI(u8 irq);
            void Remap(int offset, int offset2);

            void SetMask(u8 irq);
            void ClearMask(u8 irq);

            static u16 GetIRQreg(int ocw3);
            u16 GetIRR();
            u16 GetISR();
        }
    }
}
