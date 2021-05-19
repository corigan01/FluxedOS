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


using namespace System;
using namespace System::Memory;

#define MAX_POOL_SIZE ( 2  _MB )
#define MAX_ALLOC     (100 _KB )

i32 PMM_PAGES = 0;
i32 WorkingAddr = 0;


i32 page_table[4] __attribute__((aligned(0x20)));
i32 page_dir[512] __attribute__((aligned(0x1000))); 

#define PAGE_S 0x400000

extern i32* current_dir;
extern i32* root_dir;

// Some of the code below was made by [https://github.com/16Bitt/virtix/blob/master/src/paging.c]
// This code is not going to stay in FluxedOS, but is only here for learning. 
// I will write my own paging once I figure out how it works :)

typedef struct{
	i32 present	: 1;
	i32 rw		: 1;
	i32 user	: 1;
	i32 accessed	: 1;
	i32 dirty	: 1;
	i32 unused	: 7;
	i32 frame	: 20;
} page_t;

typedef struct{
	page_t pages[1024];
} page_table_t;

typedef struct{
	page_table_t* tables[1024];
	i32 tables_physical[1024];
	i32 physical_address;
} page_directory_t;

i32* current_dir		= 0;
i32* process_dir		= 0;
i32* root_dir			= 0;
static i32 page_dir_location	= 0;
static i32* last_page		= 0;
static i32 proc_dir		= 0;

void dump_page(i32* dir){
    kout.printf("\n\nPAGE DUMP\n==================\nPage %d \n| ");
    for (int i = 0; i < 4; i++) {
        kout.printf("%d | ", (i32)dir[i]);
    }
    kout.printf("\n\n");
}


void map_vpage_to_ppage(i32 virt, i32 physical){
	short id = virt >> 22;
	for(i32 i = 0; i < 1024; i++){
		last_page[i] = physical | 3;
		physical += 4096;
	}
	current_dir[id] = ((i32) last_page) | 3;
	last_page = (i32*) (((i32) last_page) + 4096);
}
void enable_paging(){
	asm volatile ("mov %%eax, %%cr3" :: "a" (page_dir_location));
	asm volatile ("mov %cr0, %eax");
	asm volatile ("orl $0x80000000, %eax");
	asm volatile ("mov %eax, %cr0");
}
void switch_page(i32* page_dir){
	current_dir = page_dir;
	

    dump_page(page_dir);
    dump_page((i32*) ((i32) ((*((i32*) page_dir))) & 0xFFFFF000));
	
	asm volatile("mov %0, %%cr3":: "r"(&page_dir[0]));
	i32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void Idmap_kernel() {
    for(i32 i = 0; i < 1024; i++)
		current_dir[i] = 0 | 2;

	for(i32 i = 0; i < 0x8000000; i += PAGE_S)
		map_vpage_to_ppage(i, i);	//Identity mapping the entire kernel
}

void init_paging(){
	current_dir		    = (i32*) PAGE_S;
	page_dir_location	= (i32) current_dir;
	last_page		    = (i32*) (PAGE_S + 4096);
	last_page		    = (i32*)(((i32) current_dir) + PAGE_S);
	root_dir		    = current_dir;

	
}



i32* mk_page(){					
	i32* page = (i32*) pmm::ReservePage();
	for(i32 i = 0; i < 1024; i++)
		page[i] = 2;
	return page;
}
i32* mk_page_dir(){				
	i32* dir = (i32*) pmm::ReservePage();
	
	for(i32 i = 0; i < 1024; i++){
		dir[i] =  2;
	}
	return (i32*) dir;
}

void mmap_page(i32* page_dir, i32 vpage, i32 ppage){
    kout << "idk" << endl;

	short id = vpage >> 22;
	
	i32* page = mk_page();				

	for(i32 i = 0; i < 1024; i++){
		page[i] = ppage | 2 | 1;			//User mode, RW, present
		ppage += 4096;
	}
	
	page_dir[id] = ((i32) page) | 3;	//User mode, RW, present
}


#include <System/Display/Display.hpp>
#include <System/Clock/PIT/PIT.hpp>

void Memory::init_kmalloc() {
    PMM_PAGES = MAX_POOL_SIZE / PAGE_SIZE;

    kout.printf("Kmalloc POOL SIZE %d", PMM_PAGES);
    kout << endl;

    //WorkingAddr = pmm::ReserveBook(PMM_PAGES);
    
    

    init_paging();

    map_vpage_to_ppage(0xC8B000, 753664); // why no work?
    Display::Driver::VGA dis((void*)0xC8B000);

    Idmap_kernel();
    switch_page(root_dir);

    enable_paging();
    

    dis.print_str("Happy land!!\n");
    dis.print_str("Happy land!!\n");
    dis.print_str("Happy land!!\n");

    Clock::PIT::Sleep(1000);
    
    // This does not work, idk

    
    




    kout.printf("Paging enabled! \n");


}

void* kmalloc() { return nullptr; }
void kfree(void* pointer) {};