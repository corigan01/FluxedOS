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

#include "pmm.hpp"

using namespace System;
using namespace System::Memory;

#define PageSize 4096
#define MAX_MEMORY_ENTRY 64

// The raw memory map that grub gives us
struct MemoryMap {
	multiboot_memory_map_t * Addr;
	i32 Len = 0;
} MemoryDiscriptor;

// The refined memory map with null entries removed
struct MemoryEntry {
	i32 MemoryAddr = 0;
	i32 MemoryType = 0;
	i32 Len		   = 0;
} *MemoryArray; i32 MemoryArraySize = 0;


void pmm::init(multiboot_info_t *mbt) {
	MemoryDiscriptor.Addr = (multiboot_memory_map_t*)mbt->mmap_addr; 
	MemoryDiscriptor.Len = mbt->mmap_length;

	
	for (multiboot_memory_map_t* entry = MemoryDiscriptor.Addr; entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len;) {
		
		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));
		
		if (entry->base_addr_low > 0 && (entry->length_low / (1024 * 1024)) > 0) {
			if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && MemoryArraySize == 0) {
				MemoryArray = (MemoryEntry*)entry->base_addr_low;

				entry->base_addr_low += sizeof(MemoryEntry) * MAX_MEMORY_ENTRY;
			}
			MemoryArray[MemoryArraySize++] = {.MemoryAddr = entry->base_addr_low, .MemoryType = entry->type, .Len = entry->length_low};
			
			INT_TO_STRING(Addr, MemoryArray[MemoryArraySize - 1].MemoryAddr);
			INT_TO_STRING(AddE, MemoryArray[MemoryArraySize - 1].MemoryAddr + MemoryArray[MemoryArraySize - 1].Len);
			INT_TO_STRING(LenS, MemoryArray[MemoryArraySize - 1].Len / (1024 * 1024));

			kout << "Found \'" << (MemoryArray[MemoryArraySize - 1].MemoryType == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED")  << "\' Memory: *" << Addr << " " << LenS << " MB [" << Addr << ", " << AddE << "]" << endl;
		}
	}	

}

i32 pmm::PagesAvailable() {
	i32 Pages = 0;
	for (int i = 0 ; i < MemoryArraySize; i++) {
		auto m_entry = &MemoryArray[i];

		if (m_entry->MemoryType == MULTIBOOT_MEMORY_AVAILABLE) {
			Pages += (m_entry->MemoryAddr + m_entry->Len) / PageSize; 
		}
	}
}

void pmm::ListMemory(System::Display::tty *tty) {
	kout << "-------------MEMORY DUMP-------------" << endl;

	
	for (int i = 0 ; i < MemoryArraySize; i ++) {
		auto m_entry = MemoryArray[i];

		tty->printf("Found \'%s\' Memory: (void*)%d (Len %d MB) \n", (m_entry.MemoryType == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED"), m_entry.MemoryAddr, (m_entry.Len / (1024 * 1024)));

	}		
	
			

	//tty->printf("Total Pages Available %d \n", pmm::PagesAvailable());


	kout << "-------------MEMORY DUMP-------------" << endl;
}



void RequestPage() {

}