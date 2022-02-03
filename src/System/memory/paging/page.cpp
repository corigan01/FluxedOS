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
using namespace System::Memory::Paging;

void Page::init() { 
	kout << "Starting Paging Manager" << endl;
}

page_directory_t Page::construct_page_directory() { 
	page_directory_t page_directory;
	for(int i = 0; i < 1024; i++) {
		page_directory[i] = 0;
	}
	return page_directory;
}

page_table_t Page::construct_page_table() { 
	page_table_t page_table;
	for(int i = 0; i < 1024; i++) {
		page_table[i] = 0;
	}
	return page_table;
}

void Page::map_page(page_directory_t* dir, u32 virtual_address, u32 physical_address, u32 flags) {
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		dir[page_directory_index] = (u32)construct_page_table() | flags;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = physical_address | flags;
}

void Page::map_page_table(page_directory_t* dir, u32 virtual_address, page_table_t* table, u32 flags) { 
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		dir[page_directory_index] = (u32)construct_page_table() | flags;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = (u32)table | flags;
}

void Page::map_page_directory(page_directory_t* dir, u32 virtual_address, page_directory_t* table, u32 flags) {
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		dir[page_directory_index] = (u32)construct_page_table() | flags;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = (u32)table | flags;
}

void Page::unmap_page(page_directory_t* dir, u32 virtual_address) { 
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		return;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = 0;
}	


void Page::unmap_page_table(page_directory_t* dir, u32 virtual_address) {
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		return;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = 0;
}

void Page::unmap_page_directory(page_directory_t* dir, u32 virtual_address) { 
	u32 page_directory_index = virtual_address >> 22;
	u32 page_table_index = (virtual_address >> 12) & 0x3FF;
	u32 page_offset = virtual_address & 0xFFF;
	
	if(dir[page_directory_index] == 0) {
		return;
	}
	
	page_table_t* table = (page_table_t*)dir[page_directory_index];
	table[page_table_index] = 0;
}

void Page::enable_paging(page_directory_t* page_directory) { 
	asm volatile("movl %%cr3, %%eax" : "=a"(page_directory));
	asm volatile("movl %%eax, %%cr3" : : "a"(page_directory));
	asm volatile("movl %%cr0, %%eax" : "=a"(cr0));
	asm volatile("orl $0x80000000, %%eax" : : "a"(cr0));
	asm volatile("movl %%eax, %%cr0" : : "a"(cr0));
}