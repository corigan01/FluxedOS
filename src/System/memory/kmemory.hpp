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

struct Page_Entry {
    u32 *ptr;
    u32 size;
};

namespace System 
{
    namespace Memory
    {

        void init_memory(multiboot_info_t *mbt, u32 page_start, u32 page_end);

        void SetPages(Page_Entry *pool, u32 size);

        void* kmalloc(size_t size);
        void kfree(void* ptr);

        void ConJoin(u32 m1);

        u32 MemRemaining();

        void PrintMemoryMap(int addr = -1);

        void start_debug();
        void end_debug();
    }
}


// inlines
inline void* operator new(size_t, void* p)      { return p; }
inline void* operator new[](size_t, void* p)    { return p; }
inline void* operator new(size_t size)          { return ::System::Memory::kmalloc(size); }

inline void operator delete(void* p)            { ::System::Memory::kfree(p); }
inline void operator delete(void* p, size_t)    { ::System::Memory::kfree(p); }