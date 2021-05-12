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

struct MemoryMap {
	multiboot_memory_map_t * Addr;
	i32 Len = 0;
} MemoryDiscriptor;

struct MemoryEntry {
	i32 MemoryAddr;
	i8 MemoryType;
	i32 Len;
} *MemoryArray;

void pmm::init(multiboot_info_t *mbt) {
	MemoryDiscriptor.Addr = (multiboot_memory_map_t*)mbt->mmap_addr; 
	MemoryDiscriptor.Len = mbt->mmap_length;

	multiboot_memory_map_t* entry = MemoryDiscriptor.Addr;
	while(entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len) {
		
		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));
		
		if (entry->base_addr_low > 0 && (entry->length_low / (1024 * 1024)) > 0) {
			INT_TO_STRING(Addr, entry->base_addr_low);
			INT_TO_STRING(AddE, entry->base_addr_low + entry->length_low);
			INT_TO_STRING(LenS, entry->length_low / (1024 * 1024));
			kout << "Found \'" << (entry->type == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED")  << "\' Memory: *" << Addr << " " << LenS << " MB [" << Addr << ", " << AddE << "]" << endl;


		}
	}	

}

void pmm::ListMemory(System::Display::tty *tty) {
	kout << "-------------MEMORY DUMP-------------" << endl;

	multiboot_memory_map_t* entry = MemoryDiscriptor.Addr;
	while(entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len) {
		
		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));

		INT_TO_STRING(Addr, entry->base_addr_low);
		INT_TO_STRING(LenS, entry->length_low / (1024 * 1024));

		if (entry->base_addr_low > 0 && (entry->length_low / (1024 * 1024)) > 0) {
			kout << "[*" << Addr << ", " << LenS << "]" << endl;
			tty->printf("Found \'%s\' Memory: (void*)%d (Len %d MB) \n", (entry->type == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED")  ,entry->base_addr_low, (entry->length_low / (1024 * 1024)));
		}
	}	


	kout << "-------------MEMORY DUMP-------------" << endl;
}