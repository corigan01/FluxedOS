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

void Page::init(u32 bpg) {
	uint32_t PageDir[1024] __attribute__((aligned(4096)));
	uint32_t PageTbl[1024] __attribute__((aligned(4096)));

	kout << "PageDir: " << kout.ToHex((u32)PageDir) << endl;

	for (int i = 0; i < 1024; i++) {
		PageDir[i] = 0x00000000 | 0x2;
	}

	for (int i = 0; i < 1015; i++) {
		PageTbl[i] = (i * PAGE_SIZE) | 0x3;
	}



	PageDir[0] = (u32)PageTbl | 0x3;

	kout << "COOL!" << endl;
	loadPageDirectory(PageDir);
	
	kout << "COOL" << endl;
	enablePaging();

	kout << "COO" << endl;
}