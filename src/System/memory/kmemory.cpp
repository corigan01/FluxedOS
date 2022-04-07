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
#include <System/memory/MemorySizes.hpp>
#include <System/Clock/PIT/PIT.hpp>
#include <System/memory/paging/page.hpp>
#include <lib/vector/KernelVector.hpp>
#include <lib/StanderdOperations/Operations.h>
#include <System/memory/vmm/vmm.hpp>

using namespace System;
using namespace System::Memory;

Page_Entry* Pool;
size_t Pool_Size;

bool save_alloc = false;

struct MemoryEntry
{
    u32 Start;
    u32 End;
    bool Used;
};

enum DebugStatus {
    EXACT_SIZE = 0,
    RESIZE,
    DEALLOC,
    CONJOIN,
    NEW
};

struct MemoryDebug {
    MemoryEntry entry;
    DebugStatus status;
    size_t id = 0;
};

K_Vector<MemoryEntry> MemoryMap(0);
K_Vector<MemoryDebug> DebugMap (0);

void Memory::PrintMemoryMap(int addr) {
    kout << endl;
    kout << "Kernel Memory Map: " << endl;
    kout << "\t|    BEGIN   |     END    |   SIZE  | USED |" << endl;
    kout << "\t|------------|------------|---------|------|" << endl;
    for (int i = 0; i < MemoryMap.size(); i++) {
        u32 MemorySize = (MemoryMap[i].End - MemoryMap[i].Start);
        const char* SubScript = (MemorySize >= (1 _MB) ? " MB" : (MemorySize >= (1 _KB) ? " KB" : "  B"));
        
        if (MemorySize >= (1 _MB)) MemorySize /= 1 _MB;
        if (MemorySize >= (1 _KB)) MemorySize /= 1 _KB;

        kout << "\t| 0x" << kout.ToHex(MemoryMap[i].Start) << " | 0x" << kout.ToHex(MemoryMap[i].End) 
             << " | " << MemorySize << (MemorySize > 9 ? (MemorySize > 99 ? " " : "  " ) : "   ") << SubScript <<  " | " << (MemoryMap[i].Used ? kout.RED : kout.GREEN)
             << (MemoryMap[i].Used ? "USED" : "FREE") << kout.YELLOW << " |" 
             << (addr == i ? " <-- ALLOC" : "") << endl;

        //kout << "MEMORY : " << kout.ToHex(MemoryMap[i].Start) << " --> " << kout.ToHex(MemoryMap[i].End) << ", SIZE: " << MemorySize << SubScript << "\t USED: " << (MemoryMap[i].Used ? "(USED)" : "(FREE)") << endl;
    }
    kout << "\t|------------|------------|---------|------|" << endl;

}

void Memory::init_memory(multiboot_info_t *mbt, u32 page_start, u32 page_end) {
    
    MemoryMap.ChangePointer((void*)(PAGEDIR_ADDRESS(page_start) + 1));
    // Now we need to give this memory area to MemoryMap
    // But we need to do it in a way that MemoryMap can read
    for (size_t i = page_start + 1; i < page_end; i++) {
        u32 Addr = PAGEDIR_ADDRESS(i);

        MemoryEntry entry = {
            Addr,
            Addr + (4 _MB),
            false
        };

        MemoryMap.push_back(entry);
    }

    // Now we need to conjoin memory if its the same
    // This helps us map bigger maps because we know the memory map
    // is liner
    u32 MSize = MemoryMap.size();
    for (u32 i = 0; i < MSize; i++) {
        ConJoin(0);
    }

    // Now lets print out the memory buffer
    PrintMemoryMap();

    // Ok we should be done
    kout << "Starting memory manger: " << (page_end - (page_start + 1)) * 1024 << " pages available!" << endl;

    DebugMap.construct_pointer();
}


void Memory::SetPages(Page_Entry *pool, u32 size) {
    kout << "Kernel has received new memory pool of size " << (size * PAGE_SIZE) / KB << " kbytes" << endl;

    Pool = pool;
    Pool_Size = size;

    kout << "Memory pool has been set up!" << endl;
    for (u32 i = 0; i < size; i++) {
        MemoryEntry Current_Page = {
            (u32)pool[i].ptr,
            (u32)pool[i].ptr + PAGE_SIZE,
            false
        };
        if ((u32)pool[i].ptr > 0) {
            MemoryMap.push_back(Current_Page);
        }
    }

    for (u32 i = 0; i < MemoryMap.size(); i ++) {
        kout << "MEMORY BEFORE CONJOIN : " << MemoryMap[i].Start << ", " << MemoryMap[i].End << endl;
        //ConJoin(i);
    }

    u32 MSize = MemoryMap.size();
    for (u32 i = 0; i < MSize; i++) {
        ConJoin(0);
    }
}

void Memory::ConJoin(u32 m1) {
    if ((m1 >= MemoryMap.size() && m1 < 0) || MemoryMap.size() <= 1) return;

    if (!MemoryMap[m1].Used && !MemoryMap[m1 + 1].Used) {
        if (MemoryMap[m1].End == MemoryMap[m1 + 1].Start || MemoryMap[m1].End == MemoryMap[m1 + 1].Start - 1) {
            //kout << "Joined Memory index " << m1 << " with " << m1 + 1 << " | (" << MemoryMap[m1].Start << ", " << MemoryMap[m1].End << ") --> (" << MemoryMap[m1].Start << ", " << MemoryMap[m1 + 1].End << ")" << endl;
            MemoryMap[m1].End = MemoryMap[m1 + 1].End;
            MemoryMap.pop_at(m1 + 1);
            return;
        }
    }
    
    //kout << "ERROR JOINING MEMORY (ERROR CODE): " << (m1 < MemoryMap.size() && m1 >= 0) << !MemoryMap[m1].Used
    //     << (MemoryMap[m1].End == MemoryMap[m1 + 1].Start || MemoryMap[m1].End == MemoryMap[m1].Start + 1) << endl;
}


void* Memory::kmalloc(size_t size) {
    if (size > KHEAP_MAX_ALLOC) {
        kout << "Requested allocation is too large!" << endl;
        return nullptr;
    }
    if (size == 0) { 
        kout << "Requested allocation is too small!" << endl;
        return nullptr;
    }

   //kout << "Requested size " << kout.BOLD_CYAN << size << kout.YELLOW << ". Searching..." << endl;

    for (u32 i = 0; i < MemoryMap.size(); i++) {
        //kout << "Finding Memory..." << endl;
        u32 CurrentMemorySize = MemoryMap[i].End - MemoryMap[i].Start;
        //kout << "Found Memory at loc : " << (u32)MemoryMap[i].Start << " - " << (u32)MemoryMap[i].End << " [" << ((u32)MemoryMap[i].End - (u32)MemoryMap[i].Start)  << " bytes]" << endl; 
        if (!MemoryMap[i].Used && CurrentMemorySize != 0) {
            if (CurrentMemorySize == size) {
                MemoryMap[i].Used = true;

                if (save_alloc) { // debugging mode
                    MemoryDebug debug = {MemoryMap[i], EXACT_SIZE, i};
                    DebugMap.push_back(debug);
                }

                //kout << "Found Exact Free Memory Size! " << endl;

                memset((void*)MemoryMap[i].Start, NULL, CurrentMemorySize);
                //kout << "Returning memory addr *" << (u32)MemoryMap[i].Start << " with size of " << size << endl << endl;
                return (void*)MemoryMap[i].Start;
            }
            if (CurrentMemorySize > size) {
                //kout << "Resizing Memory to fit : [" << CurrentMemorySize << "] --> {[" << size << "], [" << CurrentMemorySize - size << "]} " << endl;
                u32 MemoryStart = MemoryMap[i].Start;
                MemoryEntry NewMemoryEntry = {
                    MemoryStart,
                    MemoryStart + size,
                    true
                };
                MemoryEntry CurrentMemoryEntry = {
                    MemoryStart + size + 1,
                    MemoryMap[i].End,
                    false
                };
                MemoryMap[i] = CurrentMemoryEntry;
                MemoryMap.insert_at(i, NewMemoryEntry);

                if (save_alloc) { // debugging mode

                    for (size_t e = i + 1; e < DebugMap.size(); e++) {
                        DebugMap[e].id++;
                    }

                    MemoryDebug debug = {NewMemoryEntry, NEW, i};
                    DebugMap.push_back(debug);

                    //debug = {CurrentMemoryEntry, RESIZE, i + 1};
                    //DebugMap.push_back(debug);
                }

                //PrintMemoryMap(i);

                memset((void*)MemoryStart, NULL, size);
                //kout << "Returning memory addr *" << (u32)MemoryStart << " with size of " << size << endl << endl;
                return (void*)MemoryStart;
            }
            
        }
    }

    kout << kout.BOLD_RED << "COULD NOT FIND MEMORY, OR IS OUT OF MEMORY!" << kout.YELLOW << endl;
    Vasm("int $0x13"); // Cause fault because we are just going to page fault anyway (This is going to be a panic later)
    return nullptr;
}
void Memory::kfree(void* ptr) {
    for (u32 i = 0; i < MemoryMap.size(); i++) {
        if ((u32)ptr == MemoryMap[i].Start && MemoryMap[i].Used) {
            MemoryMap[i].Used = false;

            if (save_alloc) { // debugging mode
                MemoryDebug debug = {MemoryMap[i], DEALLOC, i};
                DebugMap.push_back(debug);
            }

            //kout << "Freed memory at " << (u32)ptr << endl;
            if (!save_alloc) {
                ConJoin(i);
                if (i > 0) {
                    ConJoin(i - 1);
                }
            }

            //ConJoin(i); // This causes a fault for some reason
            
            //PrintMemoryMap();

            return;
        }
    }
    kout << "Could not find memory to free!" << endl;
}

u32 Memory::MemRemaining() {
    u32 total_mem = 0;

    for (int i = 0; i < MemoryMap.size(); i++) {
        if (MemoryMap[i].Used) continue;

        total_mem += MemoryMap[i].End - MemoryMap[i].Start;
    }

    return total_mem;
}

u32 memory_start = 0;

void Memory::start_debug() {
    memory_start = MemRemaining();
    save_alloc = true;
}

void Memory::end_debug() {
    save_alloc = false;
    u32 memory_diff = 0;
    char sym = '-';

    if (memory_start > MemRemaining()) {
        memory_diff = memory_start - MemRemaining();
        sym = '+';
    }
    else
        memory_diff = MemRemaining() - memory_start;

    kout << endl;
    kout << "Kmalloc Debug Info" << endl;
    kout << "\tMEMORY REMAINING\t" << MemRemaining() / MB << "MB" << endl;
    kout << "\tMEMORY DIF      \t" << (sym == '+' ? kout.RED : kout.GREEN)  << sym << memory_diff << kout.YELLOW << endl;
    kout << "\tDEBUG MAP -- TOTAL EVENTS: " << DebugMap.size() << endl;
    kout << "\t\t| EVENT |  STATUS  | ID | USED |" << endl;
    kout << "\t\t|-------|----------|----|------|" << endl;
    for (int i = 0; i < DebugMap.size(); i++) {
        char* stat = "";

        switch (DebugMap[i].status) {
            case RESIZE:
                stat = "RESIZE ";
                break;
            case CONJOIN:
                stat = "CONJOIN";
                break;
            case DEALLOC:
                stat = "DEALLOC";
                break;
            case EXACT_SIZE:
                stat = "EXACT  ";
                break;
            case NEW:
                stat = "NEW    ";
                break;
            default:
                stat = (char*)kout.ToString(DebugMap[i].status);
                break;
        }
        kout << "\t\t|  " << i << (i < 10 ? "  " : (i < 100 ? " " : "")) << "  | " << stat << "  | " << DebugMap[i].id << " | " << (DebugMap[i].entry.Used ? kout.RED : kout.GREEN)
                                                       << (DebugMap[i].entry.Used ? "USED" : "FREE") << kout.YELLOW
                                                       << kout.YELLOW << " |" << endl;
    }
    kout << "\t\t|-------|----------|----|------|" << endl;

    kout << "\tSorted DEBUG MAP" << endl;
    kout << "\t\t| ID | STATUS |  MEMORY |  STAT   |" << endl;
    kout << "\t\t|----|--------|---------|---------|" << endl;
    K_Vector<u32> already_used;
    u32 last_number = 0;

    u32 size_of_last = 0;
    char* subscript_of_last = "";

    bool state_of_last = false;

    for (int i = 0; i < DebugMap.size(); i++) {
        u32 working_id = DebugMap[i].entry.Start;

        bool already_used_index = false;
        for (int e = 0; e < already_used.size(); e++) {
            if (working_id == already_used[e]) {
                already_used_index = true;
                break;
            }
        }

        if (already_used_index)
            continue;

        already_used.push_back(working_id);

        if (last_number !=DebugMap[i].entry.Start) {
            last_number = DebugMap[i].entry.Start;
            if (state_of_last)
                kout << "\t\t|    |" << kout.BOLD_RED << "   --- LEAK: " << size_of_last << " "
                                       << subscript_of_last << (size_of_last > 9 ? (size_of_last > 99 ? "" : " " ) : "  ")
                                       << " ---" << kout.YELLOW << "    |" << endl;
            //kout << "\t\t|----|--------|---------|---------|" << endl;
            kout << "\t\t|----|--------|---------|---------|" << endl;
        }

        for (int e = 0; e < DebugMap.size(); e++) {
            if (working_id == DebugMap[i].entry.Start) {
                char* stat = "";

                switch (DebugMap[e].status) {
                    case RESIZE:
                        stat = "RESIZE ";
                        break;
                    case CONJOIN:
                        stat = "CONJOIN";
                        break;
                    case DEALLOC:
                        stat = "DEALLOC";
                        break;
                    case EXACT_SIZE:
                        stat = "EXACT  ";
                        break;
                    case NEW:
                        stat = "NEW    ";
                        break;
                    default:
                        stat = (char*)kout.ToString(DebugMap[e].status);
                        break;
                }

                size_of_last = (DebugMap[e].entry.End - DebugMap[e].entry.Start);

                subscript_of_last = (char *)(size_of_last >= (1 _MB) ? " MB" : (size_of_last >= (1 _KB)
                                                                                          ? " KB" : "  B"));

                if (size_of_last >= (1 _MB)) size_of_last /= 1 _MB;
                if (size_of_last >= (1 _KB)) size_of_last /= 1 _KB;

                kout << "\t\t| " << DebugMap[e].id << " |  "
                        << (DebugMap[e].entry.Used ? kout.RED : kout.GREEN)
                        << (DebugMap[e].entry.Used ? "USED" : "FREE") << kout.YELLOW
                        << "  | " << size_of_last << (size_of_last > 9 ? (size_of_last > 99 ? " " : "  " ) : "   ")
                        << subscript_of_last << " | "
                        << stat << " |" << endl;

                state_of_last = DebugMap[e].entry.Used;
            }
        }
        kout << "\t\t|----|--------|---------|---------|" << endl;
    }
    kout << endl;
}
