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

using namespace System::Memory;

u32* current_dir		        = 0;
u32* process_dir		        = 0;
u32* root_dir			        = 0;
static u32 page_dir_location	= 0;
static u32* last_page		    = 0;
static u32 proc_dir		        = 0;

u32* Page::RootDir() {
    return root_dir;
}

void Page::init() {
    current_dir		    = (u32*) PAGE_S;
	page_dir_location	= (u32) current_dir;
	last_page		    = (u32*) (PAGE_S + 4096);
	root_dir		    = current_dir;
}
void Page::switch_page(u32* page_dir) {
    current_dir = page_dir;
	

    dump_page(page_dir);
    dump_page((u32*) ((u32) ((*((u32*) page_dir))) & 0xFFFFF000));
	
    kout.printf("Flushing page %d \n", page_dir);
	asm volatile("mov %0, %%cr3":: "r"(&page_dir[0]));
	u32 cr0;
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

void Page::map_addr(u32 v, u32 p, u8 perm) {
	// 

	char PermString[4] = "-RS";
	if (CHECK_BIT(perm, 0)) {
		PermString[0] = 'P';
	}
	if (CHECK_BIT(perm, 1)) {
		PermString[1] = 'W';
	}
	if (CHECK_BIT(perm, 2)) {
		PermString[2] = 'U';
	}


    kout.printf("\nCurrently mapping\t%d  \tto\t%d \t; Page id %d \t; Perm : %s ", v, p, v / PAGE_S, PermString);

	// Bit 0 (P) is the Present flag.
	// Bit 1 (R/W) is the Read/Write flag.
	// Bit 2 (U/S) is the User/Supervisor flag.

    //u8 perm = 0b011;

	short id = v >> 22;
	for(u32 i = 0; i < 1024; i++){
		last_page[i] = p | perm;
		p += 4096;
	}
	current_dir[id] = ((u32) last_page) | perm;
	last_page = (u32*) (((u32) last_page) + 4096);

	//pmm::ForceBook(1, p);
}
void Page::map_page(u32* page_dir, u32 vpage, u32 ppage) {
    

	short id = vpage >> 22;
	
	u32* page = mk_page();				

	for(u32 i = 0; i < 1024; i++){
		page[i] = ppage | 2 | 1;			//User mode, RW, present
		ppage += 4096;
	}
	
	page_dir[id] = ((u32) page) | 3;	    //User mode, RW, present
}

void Page::id_map(u32 PStart, u32 VStart, u32 Offset, u8 perm) {

    for (u32 i = PStart; i < Offset; i += PAGE_S) {
        map_addr(i + VStart, i, perm);
    }

}

void Page::dump_page(u32* page) {
    kout.printf("\n\nPAGE DUMP\n==================\nPage %d \n| ");
    for (int i = 0; i < 4; i++) {
        kout.printf("%d | ", (u32)page[i]);
    }
    kout.printf("\n\n");
}

u32* Page::mk_page() {
    u32* page = (u32*) pmm::ReservePage();
	for(u32 i = 0; i < 1024; i++)
		page[i] = 2;
	return page;
}

u32* Page::mk_page_dir() {
    u32* dir = (u32*) pmm::ReservePage();
	
	for(u32 i = 0; i < 1024; i++){
		dir[i] =  2;
	}
	return (u32*) dir;
}
