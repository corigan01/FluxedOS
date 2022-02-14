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

#include "page.hpp"
#include <lib/core/core.h>
#include <System/kout/kout.hpp>
#include <System/memory/pmm/pmm.hpp>
#include <System/memory/kmemory.hpp>
#include <System/cpu/cpu.hpp>
#include <lib/math/math.hpp>
#include <System/memory/staticalloc/skmalloc.hpp>

using namespace System::Memory;
using namespace System::Memory::Page;


/*
bool paging_enabled = false;
page_directory_t * kpage_dir;

void * Page::virtual2phys(page_directory_t * dir, void * virtual_addr) {
    if(!paging_enabled) {
        return (void*)(virtual_addr - LOAD_MEMORY_ADDRESS);
    }
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr), page_frame_offset = PAGEFRAME_INDEX(virtual_addr);
    if(!dir->table[page_dir_idx]) {
        kout <<"virtual2phys: page dir entry does not exist" << endl;
        return NULL;
    }
    page_table_t * table = dir->table[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        kout << "virtual2phys: page table entry does not exist" << endl;
        return NULL;
    }
    uint32_t t = table->pages[page_tbl_idx].frame;
    t = (t << 12) + page_frame_offset;
    return (void*)t;
}

void switch_page_directory(page_directory_t * page_dir, uint32_t phys, u32 tmp = 0) {
    uint32_t t;
    if(!phys)
        t =  (uint32_t)virtual2phys((page_directory_t*)tmp, page_dir);
    else
        t = (uint32_t)page_dir;

	kout << "Doing it!" << endl;
    asm volatile("mov %0, %%cr3" :: "r"(t));
}

void enable_paging() {
    uint32_t cr0, cr4;

    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    CLEAR_PSEBIT(cr4);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    SET_PGBIT(cr0);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    paging_enabled = 1;
}

void Page::allocate_page(page_directory_t * dir, uint32_t virtual_addr, uint32_t frame) {
    page_table_t * table = NULL;
    if(!dir) {
        kout << "allocate_page: page directory is empty" << endl;
        return;
    }
    // Ask pmm for a physical block, and assign the physical block to the virtual address,(left shift 12 bits for storing permission info)
    // This looks so much like the code I wrote for the virtual memory lab I wrote in a system programming class :)
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr);
    // If the coressponding page table does not exist, malloc!
    table = dir->table[page_dir_idx];
    if(!table) {  
        table = (page_table_t*)Memory::Static::skmalloc(sizeof(page_table_t));
        

        memset(table, 0, sizeof(page_table_t));
        // Remember, dumb_kmalloc returns a virtual address, but what we put into the paging structure, MUST BE, in terms of phsical address
        // Since we've mapped [0 to 4mb physical mem] to [0xc0000000 to 0xc0000000+4mb], we can get the physical addr by subtracting 0xc0000000

        uint32_t t = (uint32_t)virtual2phys(kpage_dir, table);
        dir->dir_entry[page_dir_idx].frame = t >> 12;
        dir->dir_entry[page_dir_idx].present = 1;
        dir->dir_entry[page_dir_idx].rw = 1;
        dir->dir_entry[page_dir_idx].user = 1;
        dir->dir_entry[page_dir_idx].page_size = 0;
        // Leave a reference here so that later we can access this table
        dir->table[page_dir_idx] = table;
    }

    // If the coressponding page does not exist, allocate_block!
    if(!table->pages[page_tbl_idx].present) {
        uint32_t t;
        // Normally, we'll allocate frames from physical memory manager, but sometimes it's useful to be able to set any frame(for example, share memory between process)
        //if(frame)
        t = frame;
        //else
            //t = allocate_block();
        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = 1;
        table->pages[page_tbl_idx].user = 1;
    }
}*/



EXTNC_ void loadPageDirectory(unsigned int*);
EXTNC_ void enablePaging();

void PrintPageDir(u32 bpg) {
    bpg += LOAD_MEMORY_ADDRESS;
    kout << "PGD: " << kout.ToHex(bpg) << endl;
    

    kout << endl <<
    "Reading Paging Dir: \n" <<
    "\tDIR ADDR     *0x" << kout.ToHex(bpg) << endl;
    
    u32 DetectedAddr = 0;

    for (int i = 0; i < 64; i ++) {
        kout << "\t";
        for (int e = 0; e < 16; e++) {

            u32 OutAdd = ((u32*)(bpg & 0xFFFFF000))[i + (e * 64)];

            kout << (OutAdd > 0 ? kout.GREEN : kout.YELLOW) << "[" << i + (e * 64) << "]:" << ((i + (e * 64)) >= 10 ? ((i + (e * 64)) >= 100 ? " " : "  ") : "   ") << "0x" << kout.ToHex(OutAdd) << " " << kout.YELLOW;

            DetectedAddr = (OutAdd > 1) ? (i + (e * 64)) : DetectedAddr;
        }
        kout << endl;
    }

    if (DetectedAddr == 0) return;

    u32 bpt = (u32)((u32*)(bpg & 0xFFFFF000))[DetectedAddr] + LOAD_MEMORY_ADDRESS;

    kout << "Reading Paging Table; \n\tTBL ADDR:   *0x" << kout.ToHex(bpt) << endl;

    for (int i = 0; i < 64; i ++) {
        kout << "\t";
        for (int e = 0; e < 16; e++) {

            u32 OutAdd = ((u32*)(bpt & 0xFFFFF000))[i + (e * 64)];

            kout << (OutAdd > 0 ? kout.GREEN : kout.YELLOW) << "[" << i + (e * 64) << "]:" << ((i + (e * 64)) >= 10 ? ((i + (e * 64)) >= 100 ? " " : "  ") : "   ") << "0x" << kout.ToHex(OutAdd) << " " << kout.YELLOW;

        }
        kout << endl;
    } 

}

u32 VirtToPhy(u32 PageDir, u32 addr) {
    // Dir index 0-1024, this points to which table we are looking at
    u32 DirEntryIdx = ((u32)addr / (4 _MB));

    // Table index 0-1024, this points to which entry we are looking for
    u32 TableIdx = ((u32)addr - (DirEntryIdx * 4 _MB)) / (4 _KB); 

    // This is the actual page table addr
    u32 Table = (u32)((u32*)((u32)PageDir & 0xFFFFF000))[DirEntryIdx] + LOAD_MEMORY_ADDRESS;

    // This is the phys addr
    u32 Paddr = ((u32*)(Table & 0xFFFFF000))[TableIdx] & 0xFFFFF000;

    return Paddr;
}

void CopyBootDirectory(u32 bootdir, u32 newdir, u32 loadmem) {
    u32 DirEntryIdx = ((u32)loadmem / (4 _MB));
    u32 bpt = (u32)((u32*)(bootdir & 0xFFFFF000))[DirEntryIdx] + loadmem;


    ((u32*)newdir)[DirEntryIdx] = bpt - loadmem;
}

void SendTableToDirectory(u32 dir, u32 table, u32 index) {
    ((u32*)dir)[index] = table;
}

void ClearAllEntries(u32 dir) {
    memset((void*)dir, NULL, 1024);
}

void MemMap(u32 table, u32 perm, u32 offset) {
    u32* tbl = (u32*)table;

    for (int i = 0; i < offset; i++) {
        tbl[i] = ((pmm::ReservePage() - LOAD_MEMORY_ADDRESS) & 0xFFFFF000) | perm;
    }
}

void Page::init(u32 bpg) {
    
    // In the boot loader we reserve 20KB of memory for this new page dir / table
    // This is reserved to us in the virtual memory range, so we need to figure out
    // where this memory points to later on
    u32 PageDir = (u32)Memory::Static::skmalloc(1024 * sizeof(u32)); 

    // This will be a new addr for the heap (WIP)
    u32 PageTbl = (u32)Memory::Static::skmalloc(1024 * sizeof(u32)); 


    // Make sure we clear the PageDir
    ClearAllEntries(PageDir);
    ClearAllEntries(PageTbl);



    CopyBootDirectory(bpg, PageDir, LOAD_MEMORY_ADDRESS);

    MemMap(PageTbl, SUPER_USER_MEMORY | PRESENT_FLAG | READ_WRITE_ENABLED, 1024);
    SendTableToDirectory(PageDir, PageTbl, 769);
    
    

    // Tell the MMU to use this new Page Dir
    loadPageDirectory((u32*)(VirtToPhy(PageDir, PageDir)));
    
    // Make sure Paging is enabled
    enablePaging();

    // AND DIE BECAUSE THIS TOOK SO LONG
}