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

#include "skmalloc.hpp"

using namespace System;
using namespace System::Memory;

u32* MemoryAddr;
u32 Offset;
u32 Alloc;


void StaticHeap::init(void* startloc, u32 offset) {
    kout << "NEW SKMALLOC: " <<
            "\n\tSIZE  : " << (offset / 1024) << "KB" <<  
            "\n\tSTART : " << kout.ToHex((u32)startloc) << endl;
    MemoryAddr = (u32*)startloc;
    Offset = offset;
    Alloc = 0;
}

void* StaticHeap::skmalloc(size_t size) {
    if (Alloc + size > Offset) {
        kout << "skmalloc: out of memory" << endl;
        return nullptr;
    }

    kout << "skmalloc: " << kout.ToHex((u32)(MemoryAddr + Alloc)) << " --> Size: " << size << endl;

    u8* ret = (u8*)MemoryAddr + Alloc;
    Alloc += size + 1;


    return (void*)ret;
}