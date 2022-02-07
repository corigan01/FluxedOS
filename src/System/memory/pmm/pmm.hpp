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
#include <System/Display/Display.hpp>
#include <System/memory/paging/page.hpp>

#define PAGE_SIZE PAGE_S
#define MAX_MEMORY_ENTRY 64

namespace System
{
    namespace Memory
    {
        namespace pmm
        {
            void init(multiboot_info_t *mbt);
            void TestMemory(System::Display::tty *tty);
            u32 PagesAvailable();
            u32 RequestInitial();

            u32 ReservePage();
            void freeBlock(u32 addr);

            // This is to reserve many pages at once, and pages makes a book :)
            u32 ReserveBook(u16 PagesNumber);
            void freeBook(u32 Addr, u16 Pages);

            // Force alloc
            u32 ForceBook(u16 PagesNumber, u32 offset);

        } // namespace pmm
        
    } // namespace Memory
    
}