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

#include "kmemory.hpp"
#include <System/memory/pmm/pmm.hpp>
#include <System/memory/MemorySizes.hpp>
#include <System/Display/Display.hpp>
#include <System/Clock/PIT/PIT.hpp>
#include <System/memory/paging/page.hpp>
#include <lib/vector/vector.hpp>

using namespace System;
using namespace System::Memory;

#define MAX_POOL_SIZE ( 8  _MB )
#define MAX_ALLOC     (100 _KB )

Page_Entry* Pool;
size_t Pool_Size;

struct MemoryEntry
{
    i32 Start;
    i32 End;
    bool Used;
};
Vector<MemoryEntry> MemoryMap;


void Memory::init_memory(multiboot_info_t *mbt) {
    pmm::init(mbt);
    MemoryMap.ChangePointer((void*)pmm::ReservePage());
    kout << "Starting memory manger: " << pmm::PagesAvailable() << " pages available!" << endl;
}

Page_Entry Memory::map_page(Permission_Entry perm) {
    Page_Entry entry;
    entry.ptr = (void*)pmm::ReservePage();
    entry.perm = perm;
    entry.size = PAGE_S;

    kout << "Mapped page at: " << (i32)entry.ptr << " with permission: " << perm.perm << endl;

    return entry;
}
bool Memory::unmap_page(Page_Entry page) {
    pmm::freePage((i32)page.ptr);

    // TODO: Check if page is still mapped with paging
    kout << "Unmapped page at: " << (i32)page.ptr << endl;

    return true;
}

void Memory::PagePool(Page_Entry *pool, i32 size) {
    kout << "Kernel has received new memory pool of size " << (size * PAGE_SIZE) / KB << " kbytes" << endl;

    Pool = pool;
    Pool_Size = size;

    kout << "Memory pool has been set up!" << endl;
    for (i32 i = 0; i < size; i++) {
        MemoryEntry Current_Page = {
            (i32)pool[i].ptr,
            (i32)pool[i].ptr + PAGE_SIZE,
            false
        };
        MemoryMap.push_back(Current_Page);
    }

    for (i32 i = 0; i < MemoryMap.size(); i ++) {
        kout << MemoryMap[i].Start << " --> " << MemoryMap[i].End << endl;
    }
}

void* Memory::kmalloc(size_t size) {
    if (size > MAX_ALLOC) {
        kout << "Requested allocation is too large!" << endl;
        return NULL;
    }
    if (size == 0) { 
        kout << "Requested allocation is too small!" << endl;
        return NULL;
    }

    kout << "LOOKING FOR MEMORY" << endl;

    for (i32 i = 0; i < MemoryMap.size(); i++) {
        //kout << "Finding Memory..." << endl;
        kout << "Found Unused Memory at loc : " << (i32)MemoryMap[i].Start << " - " << (i32)MemoryMap[i].End << " [" << ((i32)MemoryMap[i].End - (i32)MemoryMap[i].Start) / KB << " kb]" << endl; 
        if (!MemoryMap[i].Used) {
            i32 CurrentMemorySize = MemoryMap[i].End - MemoryMap[i].Start;

            if (CurrentMemorySize == size) {
                MemoryMap[i].Used = 1;

                kout << "Found Exact Free Memory Size! " << endl;

                memset((void*)MemoryMap[i].Start, NULL, CurrentMemorySize);
                return (void*)MemoryMap[i].Start;
            }
            if (CurrentMemorySize > size) {
                auto MemoryStart = MemoryMap[i].Start;

                MemoryEntry NewMemoryEntry = {
                    MemoryStart,
                    MemoryStart + size,
                    1
                };
                MemoryMap[i] = {
                    MemoryStart + size + 1,
                    MemoryMap[i].End,
                    MemoryMap[i].Used
                };
                MemoryMap.insert_at(i - 1, NewMemoryEntry);

                kout << "Resizing Memory to fit : [" << CurrentMemorySize << "] --> {[" << size << "], [" << CurrentMemorySize - size << "]} " << endl;

                memset((void*)MemoryStart, NULL, size);
                return (void*)MemoryStart;
            }
            
        }
    }

    // [=] [+] <-- Free space [=====] [++++++++] [=====] ++++++++++++++++++++++++++++++++++++++]
    // [=] need to find this free
    // [=] [=] <--- Now used  [=====] [++++++++] [=====] [++++++++++++++++++++++++++++++++++++++]
    kout << "COULD NOT FIND MEMORY, OR IS OUT OF MEMORY!" << endl;
    return (void*)NULL;
}
void Memory::kfree(void* ptr) {

}