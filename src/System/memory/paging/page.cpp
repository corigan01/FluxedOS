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
#include <System/kout/kout.hpp>
#include <System/memory/pmm/pmm.hpp>

using namespace System::Memory;

i32* current_dir		        = 0;
i32* process_dir		        = 0;
i32* root_dir			        = 0;
static i32 page_dir_location	= 0;
static i32* last_page		    = 0;
static i32 proc_dir		        = 0;

i32* Page::RootDir() {
    return root_dir;
}

void Page::init() {
    current_dir		    = (i32*) PAGE_S;
	page_dir_location	= (i32) current_dir;
	last_page		    = (i32*) (PAGE_S + 4096);
	root_dir		    = current_dir;
}
void Page::switch_page(i32* page_dir) {
    current_dir = page_dir;
	

    dump_page(page_dir);
    dump_page((i32*) ((i32) ((*((i32*) page_dir))) & 0xFFFFF000));
	
    kout.printf("Flushing page %d \n", page_dir);
	asm volatile("mov %0, %%cr3":: "r"(&page_dir[0]));
	i32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}
void Page::enable_paging() {
    kout << "Attempting to enable paging" << endl;

	asm volatile ("mov %%eax, %%cr3" :: "a" (page_dir_location));
	asm volatile ("mov %cr0, %eax");
	asm volatile ("orl $0x80000000, %eax");
	asm volatile ("mov %eax, %cr0");

    kout.printf("Paging enabled for dir %d! \n", page_dir_location);
}

void Page::map_addr(i32 v, i32 p) {
    kout.printf("Currently mapping\t%d  \tto\t%d \t; Page id %d \n", v, p, v / PAGE_S);

	// Bit 0 (P) is the Present flag.
	// Bit 1 (R/W) is the Read/Write flag.
	// Bit 2 (U/S) is the User/Supervisor flag.

    i8 perm = 0b011;

	short id = v >> 22;
	for(i32 i = 0; i < 1024; i++){
		last_page[i] = p | perm;
		p += 4096;
	}
	current_dir[id] = ((i32) last_page) | perm;
	last_page = (i32*) (((i32) last_page) + 4096);
}
void Page::map_page(i32* page_dir, i32 vpage, i32 ppage) {
    kout << "idk" << endl;

	short id = vpage >> 22;
	
	i32* page = mk_page();				

	for(i32 i = 0; i < 1024; i++){
		page[i] = ppage | 2 | 1;			//User mode, RW, present
		ppage += 4096;
	}
	
	page_dir[id] = ((i32) page) | 3;	    //User mode, RW, present
}

void Page::id_map(i32 PStart, i32 VStart, i32 Offset) {

    for (i32 i = PStart; i < Offset; i += PAGE_S) {
        map_addr(i + VStart, i);
    }

}

void Page::dump_page(i32* page) {
    kout.printf("\n\nPAGE DUMP\n==================\nPage %d \n| ");
    for (int i = 0; i < 4; i++) {
        kout.printf("%d | ", (i32)page[i]);
    }
    kout.printf("\n\n");
}

i32* Page::mk_page() {
    i32* page = (i32*) pmm::ReservePage();
	for(i32 i = 0; i < 1024; i++)
		page[i] = 2;
	return page;
}

i32* Page::mk_page_dir() {
    i32* dir = (i32*) pmm::ReservePage();
	
	for(i32 i = 0; i < 1024; i++){
		dir[i] =  2;
	}
	return (i32*) dir;
}