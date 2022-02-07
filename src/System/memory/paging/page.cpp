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

// Defined in kheap.c
void * heap_start, * heap_end, * heap_max, * heap_curr;
int kheap_enabled = 0;

// Where we want to place all the paging structure data
int paging_enabled = 0;

page_directory_t * kpage_dir;

page_directory_t * boot_page_directory;
/*
 * Convert virtual address to physical address
 * If it's the temp page dir, simply subtract 0xC0000000 since we do the page mapping manually in entry.asm
 * Otherwise, search the whole page table.
 * */
void* Page::virtual2phys(page_directory_t * dir, void * virtual_addr) {
    if(!paging_enabled) {
        return (void*)(virtual_addr - LOAD_MEMORY_ADDRESS);
    }
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr), page_frame_offset = PAGEFRAME_INDEX(virtual_addr);
    if(!dir->ref_tables[page_dir_idx]) {
        kout.printf("virtual2phys: page dir entry does not exist\n");
        return NULL;
    }
    page_table_t * table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        kout.printf("virtual2phys: page table entry does not exist\n");
        return NULL;
    }
    uint32_t t = table->pages[page_tbl_idx].frame;
    t = (t << 12) + page_frame_offset;
    return (void*)t;
}

/*
 * A dumb malloc, just to help building the paging data structure for the first 4mb that our kernel uses
 * It only manages memory from the end of pmm bitmap, to 0xC0400000, approximately 2mb.
 * */
void* Page::dumb_kmalloc(uint32_t size, int align) {
	kout << "dumb_kmalloc: " << size << " bytes\n";
    void * ret = System::Memory::Static::skmalloc(size);
    // If it's not aligned, align it first
    if(align && !IS_ALIGN(ret))
        ret = (void*)PAGE_ALIGN(ret);
    return ret;
}

/*
 * Allocate a set of pages specified by the region
 * */
void Page::allocate_region(page_directory_t * dir, uint32_t start_va, uint32_t end_va, int iden_map, int is_kernel, int is_writable) {
    uint32_t start = start_va & 0xfffff000;
    uint32_t end = end_va & 0xfffff000;
    while(start <= end) {
        if(iden_map)
            allocate_page(dir, start, start / PAGE_SIZE, is_kernel, is_writable);
        else
            allocate_page(dir, start, 0, is_kernel, is_writable);
        start = start + PAGE_SIZE;
    }
}

/*
 * Allocate a frame from pmm, write frame number to the page structure
 * You may notice that we've set the both the PDE and PTE as user-accessible with r/w permissions, because..... we don't care security
 * */
void Page::allocate_page(page_directory_t * dir, uint32_t virtual_addr, uint32_t frame, int is_kernel, int is_writable) {
    page_table_t * table = NULL;
    if(!dir) {
        kout.printf("allocate_page: page directory is empty\n");
        return;
    }
    // Ask pmm for a physical block, and assign the physical block to the virtual address,(left shift 12 bits for storing permission info)
    // This looks so much like the code I wrote for the virtual memory lab I wrote in a system programming class :)
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr);
    // If the coressponding page table does not exist, malloc!
    table = dir->ref_tables[page_dir_idx];
    if(!table) {
        if(!kheap_enabled)
            table = (page_table_t *)dumb_kmalloc(sizeof(page_table_t), 1);
        else
            table = (page_table_t *)kmalloc(sizeof(page_table_t));

        memset(table, 0, sizeof(page_table_t));
        // Remember, dumb_kmalloc returns a virtual address, but what we put into the paging structure, MUST BE, in terms of phsical address
        // Since we've mapped [0 to 4mb physical mem] to [0xc0000000 to 0xc0000000+4mb], we can get the physical addr by subtracting 0xc0000000

        uint32_t t = (uint32_t)virtual2phys(kpage_dir, table);
        dir->tables[page_dir_idx].frame = t >> 12;
        dir->tables[page_dir_idx].present = 1;
        dir->tables[page_dir_idx].rw = 1;
        dir->tables[page_dir_idx].user = 1;
        dir->tables[page_dir_idx].page_size = 0;
        // Leave a reference here so that later we can access this table
        dir->ref_tables[page_dir_idx] = table;
    }

    // If the coressponding page does not exist, allocate_block!
    if(!table->pages[page_tbl_idx].present) {
        uint32_t t;
        // Normally, we'll allocate frames from physical memory manager, but sometimes it's useful to be able to set any frame(for example, share memory between process)
        if(frame)
            t = frame;
        else
            t = pmm::ReservePage();
        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = 1;
        table->pages[page_tbl_idx].user = 1;
    }
}

/*
 * Free all frames within the region
 * */
void Page::free_region(page_directory_t * dir, uint32_t start_va, uint32_t end_va, int free) {
    uint32_t start = start_va & 0xfffff000;
    uint32_t end = end_va & 0xfffff000;
    while(start <= end) {
        free_page(dir, start, 1);
        start = start + PAGE_SIZE;
    }
}

/*
 * Find the corresponding page table entry, and set frame to 0
 * Also, clear corresponding bit in pmm bitmap
 * @parameter free:
 *      0: only clear the page table entry, don't actually free the frame
 *      1 : free the frame
 * */
void Page::free_page(page_directory_t * dir, uint32_t virtual_addr, int free) {
    if(dir == boot_page_directory) return;
    uint32_t page_dir_idx = PAGEDIR_INDEX(virtual_addr), page_tbl_idx = PAGETBL_INDEX(virtual_addr);
    if(!dir->ref_tables[page_dir_idx]) {
        kout.printf("free_page: page dir entry does not exist\n");
        return;
    }
    page_table_t * table = dir->ref_tables[page_dir_idx];
    if(!table->pages[page_tbl_idx].present) {
        kout.printf("free_page: page table entry does not exist\n");
        return;
    }
    // The table entry is found !
    if(free)
        pmm::freeBlock(table->pages[page_tbl_idx].frame);
    table->pages[page_tbl_idx].present = 0;
    table->pages[page_tbl_idx].frame = 0;
}

/*
 * Remap memory used by the kernel, and enable paging, again
 * */
void Page::paging_init(u32* bitmap, u32 bitmap_size, u32 boot_page_dir) {
    /*
     * Right now, we have a temporary page directory sitting in the kernel's data section
     * I don't like that... Instead, build a new set of paging structures in the memory outside of kernel data/code, by calling our physical memory manager
     * */

	// Register page fault handler, do it later
	kout << TRACE_INFO << "Registering page fault handler\n";
    System::CPU::ISR::installISR(14, page_fault_handler);

	kout << TRACE_INFO << "Boot Page Dir set" << endl;
	boot_page_directory = (page_directory_t *)boot_page_dir;


    // Allocate a page directory and set it to all zeros(don't need to allocate explicitly because in pmm_init, we already set aside first 4mb for kernel)
	kout << TRACE_INFO << "Kpage Dir Set" << endl;
    kpage_dir = (page_directory_t *)dumb_kmalloc(sizeof(page_directory_t), 1);
    memset(kpage_dir, 0, sizeof(page_directory_t));

    // Now, map 4mb begining from 0xC0000000 to 0xC0400000(should corresponding to first 1024 physical blocks, so MAKE SURE pmm bitmap is all clear at this point)
	kout << TRACE_INFO << "Alloc Pages" << endl;
	for (int i = LOAD_MEMORY_ADDRESS; i < LOAD_MEMORY_ADDRESS + 4 _MB; i += PAGE_SIZE) {
		kout << TRACE_INFO << "\tPAGE: 0x" << kout.ToHex(i) << endl;
		allocate_page(kpage_dir, i, 0, 1, 1);
	}

    // Map some memory after 0xc0400000 as kernel heeap ? do it later.
    //i = LOAD_MEMORY_ADDRESS + 4 _MB;
    //while(i < LOAD_MEMORY_ADDRESS + 4 _MB + KHEAP_INITIAL_SIZE) {
    //    allocate_page(kpage_dir, i, 0, 1, 1);
    //    i = i + PAGE_SIZE;
    //}

    

    // Load kernel directory
    switch_page_directory(kpage_dir, 0);

    // Enable Paging (remember to set cr4 to disable 4mb pages too)
    enable_paging();
    // Identity map the first
    allocate_region(kpage_dir, 0x0, 0x10000, 1, 1, 1);
}

/*
 * Switch page directory,
 * phys : Is the address given physical or virtual ?
 * */
void Page::switch_page_directory(page_directory_t * page_dir, uint32_t phys) {
    uint32_t t;
    if(!phys)
        t = (uint32_t)virtual2phys(boot_page_directory, page_dir);
    else
        t = (uint32_t)page_dir;
    asm volatile("mov %0, %%cr3" :: "r"(t));
}


/*
 * Enable paging, turn off PSE bit first as it was turned on by entry.asm when kernel was loading
 * Then enable PG Bit in cr0
 * */
void Page::enable_paging() {
    uint32_t cr0, cr4;

    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    CLEAR_PSEBIT(cr4);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    SET_PGBIT(cr0);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    paging_enabled = 1;
}

void * Page::ksbrk(int size) {
    void * runner = NULL;
    void * new_boundary;
    void * old_heap_curr;
restart_sbrk:
    old_heap_curr = heap_curr;
    // Note: what if this is shrinking ?
    if(size == 0) {
        goto ret;
    }
    else if(size > 0) {
        new_boundary = heap_curr + (uint32_t)size;
        // If new size is smaller or equal to end, simply add size to heap_curr and return the old heap_curr
        if(new_boundary <= heap_end)
            goto update_boundary;
        // Else if new size is greater than heap_max, then return NULL
        else if(new_boundary > heap_max)
            return NULL;
        // Else if new size is in range (heap_end, heap_max], then
        else if(new_boundary > heap_end) {
            //      a) expand the heap by getting more pages
            runner = heap_end;
            while(runner < new_boundary) {
                allocate_page(kpage_dir, (uint32_t)runner, 0, 1, 1);
                runner = runner +  PAGE_SIZE;
            }
            // Put away the page table first, then sbrk user-requested data again
            if(old_heap_curr != heap_curr) {
                goto restart_sbrk;
            }
            heap_end = runner;
            goto update_boundary;
        }
    }
    else if(size < 0){
        // Free as many pages as possible, then update heap_end, heap_curr and return old_heap_curr
        new_boundary = (void*)((uint32_t)heap_curr - (uint32_t)math::abs(size));
        if(new_boundary < heap_start + HEAP_MIN_SIZE) {
            new_boundary = heap_start + HEAP_MIN_SIZE;
        }
        runner = heap_end - PAGE_SIZE;
        while(runner > new_boundary) {
            free_page(kpage_dir, (uint32_t)runner, 1);
            runner = runner - PAGE_SIZE;
        }
        heap_end = runner + PAGE_SIZE;
        goto update_boundary;
    }
update_boundary:
    heap_curr = new_boundary;
ret:
    return old_heap_curr;
}

/*
 * Copy a page directory
 * */
void Page::copy_page_directory(page_directory_t * dst, page_directory_t * src) {
    for(uint32_t i = 0; i < 1024; i++) {
        if(kpage_dir->ref_tables[i] == src->ref_tables[i]) {
            // Link kernel pages
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        }
        else {
            // For non-kernel pages, copy the pages (for example, when forking process, you don't want the parent process mess with child process's memory)
            dst->ref_tables[i] = copy_page_table(src, dst, i, src->ref_tables[i]);
            uint32_t phys = (uint32_t)virtual2phys(src, dst->ref_tables[i]);
            dst->tables[i].frame = phys >> 12;
            dst->tables[i].user = 1;
            dst->tables[i].rw = 1;
            dst->tables[i].present = 1;
        }
    }
}
/*
 * Copy a page directory
 * */
page_table_t * Page::copy_page_table(page_directory_t * src_page_dir, page_directory_t * dst_page_dir, uint32_t page_dir_idx, page_table_t * src) {
    page_table_t * table = (page_table_t*)kmalloc(sizeof(page_table_t));
    for(int i = 0; i < 1024; i++) {
        if(!table->pages[i].frame)
            continue;
        // Source frame's virtual address
        uint32_t src_virtual_address = (page_dir_idx << 22) | (i << 12) | (0);
        // Destination frame's virtual address
        uint32_t dst_virtual_address = src_virtual_address;
        // Temporary virtual address in current virtual address space
        uint32_t tmp_virtual_address = 0;

        // Allocate a frame in destination page table
        allocate_page(dst_page_dir, dst_virtual_address, 0, 0, 1);
        // Now I want tmp_virtual_address and dst_virtual_address both points to the same frame
        allocate_page(src_page_dir, tmp_virtual_address, (uint32_t)virtual2phys(dst_page_dir, (void*)dst_virtual_address), 0, 1);
        if (src->pages[i].present) table->pages[i].present = 1;
        if (src->pages[i].rw)      table->pages[i].rw = 1;
        if (src->pages[i].user)    table->pages[i].user = 1;
        if (src->pages[i].accessed)table->pages[i].accessed = 1;
        if (src->pages[i].dirty)   table->pages[i].dirty = 1;
        memcpy((void*)tmp_virtual_address, (void*)src_virtual_address, PAGE_SIZE);
        // Unlink frame
        free_page(src_page_dir, tmp_virtual_address, 0);
    }
    return table;
}


/* Print out useful information when a page fault occur
 * */
void Page::page_fault_handler(register_t * reg) {
    asm volatile("sti");
    kout << kout.RED;
    kout.printf("Page fault ----\n");

    // Gather fault info and print to screen
    uint32_t faulting_addr;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_addr));
    uint32_t present = reg->err_code & ERR_PRESENT;
    uint32_t rw = reg->err_code & ERR_RW;
    uint32_t user = reg->err_code & ERR_USER;
    uint32_t reserved = reg->err_code & ERR_RESERVED;
    uint32_t inst_fetch = reg->err_code & ERR_INST;

    kout.printf("Possible causes: [ ");
    if(!present) kout.printf("Page not present ");
    if(rw) kout.printf("Page is read only ");
    if(user) kout.printf("Page is read only ");
    if(reserved) kout.printf("Overwrote reserved bits ");
    if(inst_fetch) kout.printf("Instruction fetch ");
    kout.printf("]\n");

	kout << kout.YELLOW;

   
}