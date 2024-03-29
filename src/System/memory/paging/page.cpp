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

EXTNC_ void loadPageDirectory(unsigned int*);
EXTNC_ void enablePaging();
EXTNC_ void flushPages();

void Page::FlushPages() {
    flushPages();
}

void Page::PrintPageDir(u32 bpg, u32 vtable) {
    kout << "PGD: " << kout.ToHex(vtable) << endl;
    
    for (int i = 0; i < 64; i ++) { 
        kout << "\t";
        for (int e = 0; e < 16; e++) {

            u32 OutAdd = ((u32*)(vtable))[i + (e * 64)];

            kout << (OutAdd > 0 ? kout.GREEN : kout.YELLOW) << "[" << i + (e * 64) << "]:" << ((i + (e * 64)) >= 10 ? ((i + (e * 64)) >= 100 ? " " : "  ") : "   ") << "0x" << kout.ToHex(OutAdd) << " " << kout.YELLOW;

        }
        kout << endl;
    }

    kout << endl <<
    "Reading Paging Dir: \n" <<
    "\tDIR ADDR     *0x" << kout.ToHex(bpg) << endl;
    
    u32 DetectedAddr[10] = {};
    u32 DetectedSize = 0;

    for (int i = 0; i < 64; i ++) {
        kout << "\t";
        for (int e = 0; e < 16; e++) {

            u32 OutAdd = ((u32*)(bpg & 0xFFFFF000))[i + (e * 64)];

            kout << (OutAdd > 0 ? kout.GREEN : kout.YELLOW) << "[" << i + (e * 64) << "]:" << ((i + (e * 64)) >= 10 ? ((i + (e * 64)) >= 100 ? " " : "  ") : "   ") << "0x" << kout.ToHex(OutAdd) << " " << kout.YELLOW;

            if (OutAdd > 1) DetectedAddr[DetectedSize++] = (i + (e * 64));
        }
        kout << endl;
    }


    if (DetectedAddr[0] == 0) return;

    for (int e = 0; e < DetectedSize; e++) {
        u32 bpt = (u32)((u32*)(vtable))[DetectedAddr[e]];

        kout << "Reading Paging Table; \n\tTBL ADDR:   *0x" << kout.ToHex(bpt) << "\n\tTBL VADDR:   *0x" << kout.ToHex((u32)PAGEDIR_ADDRESS(DetectedAddr[e])) << endl;

        for (int i = 0; i < 64; i ++) {
            kout << "\t";
            for (int e = 0; e < 16; e++) {

                u32 OutAdd = ((u32*)(bpt & 0xFFFFF000))[i + (e * 64)];

                kout << (OutAdd > 0 ? kout.BLUE : kout.YELLOW) << "[" << i + (e * 64) << "]:" << ((i + (e * 64)) >= 10 ? ((i + (e * 64)) >= 100 ? " " : "  ") : "   ") << "0x" << kout.ToHex(OutAdd) << " " << kout.YELLOW;

            }
            kout << endl;
        } 
    }

    

}

typedef u32 page_dir_t;
typedef u32 page_tbl_t;


// Converts a Virt addr to a Phys one
u32 Page::VirtToPhy(page_dir_t PageDir, page_dir_t vpageDir, u32 addr) {
    u32 DirectoryIndex = PAGEDIR_INDEX(addr);
    u32 TableIndex = PAGETBL_INDEX(addr);

    u32 address = ((u32*)(((u32*)(vpageDir & 0xFFFFF000))[DirectoryIndex] & 0xFFFFF000))[TableIndex] & 0xFFFFF000;

    kout << "0x" << kout.ToHex(addr) << " [" << DirectoryIndex << "][" << TableIndex << "] 0x" << kout.ToHex(address) << endl;
    
    return address;
}

// This will move the boot page dir into a new page dir
void Page::CopyBootDirectory(page_dir_t bootdir, page_dir_t newdir, u32 loadmem) {
    u32 DirEntryIdx = ((u32)loadmem / (4 _MB));
    u32 bpt = (u32)((u32*)(bootdir & 0xFFFFF000))[DirEntryIdx] + loadmem;


    ((u32*)newdir)[DirEntryIdx] = bpt - loadmem;
}

// This will just move a table into the dir
void Page::SendTableToDirectory(page_dir_t dir, page_dir_t vdir, page_tbl_t table, u32 index, u32 perms) {
    
    ((u32*)vdir)[index] = (u32)table; 
    ((u32*)dir)[index] = VirtToPhy(dir, vdir, table) | perms;
    

    flushPages();
}

// Makes sure all entries are clear
void Page::ClearAllEntries(page_dir_t dir) {
    memset((void*)dir, NULL, 1024);
}

// Maps a region of a table using the pmm
void Page::MapTableRegion(page_tbl_t table, u32 perm, u32 offset) {
    u32* tbl = (u32*)table;

    for (int i = 0; i < offset; i++) {
        tbl[i] = ((pmm::ReservePage()) & 0xFFFFF000) | perm;
    }
}

// BOOT
u32 Page::BOOT_VirtToPhy(page_dir_t PageDir, u32 addr) {
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


page_dir_t PageDir;

extern u32 _kernel_end;

void Page::init(u32 bpg) {
    // In the boot loader we reserve 40KB of memory for this new page dir / table
    // This is reserved to us in the virtual memory range, so we need to figure out
    // where this memory points to later on
    PageDir = PAGE_ALIGN((u32)Memory::StaticHeap::skmalloc(1024 * sizeof(u32) * 2)); 

    // Make sure we clear the PageDir
    ClearAllEntries(PageDir);

    // This will copy the important infomation from the boot page table into
    // a new page table that we can work with. This new page table then can
    // alloc new regions. 
    CopyBootDirectory(bpg, PageDir, LOAD_MEMORY_ADDRESS);
    
    // Tell the MMU to use this new Page Dir
    loadPageDirectory((u32*)(BOOT_VirtToPhy(PageDir, PageDir)));
    
    // Make sure Paging is enabled
    enablePaging();

}

page_dir_t Page::GetPageDir() {
    return PageDir;
}