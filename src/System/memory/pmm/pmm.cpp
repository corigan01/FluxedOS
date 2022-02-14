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
#include <lib/bitmap/bitmap.hpp>

using namespace System;
using namespace System::Memory;

// The raw memory map that grub gives us
struct RawMemoryVector {
	multiboot_memory_map_t * Addr;
	u32 Len = 0;
} MemoryDiscriptor;

// The refined memory map with null entries removed
struct MemoryEntry {
	u32 MemoryAddr = 0;
	u32 MemoryType = 0;
	u32 Len		   = 0;
} *MemoryArray; u32 MemoryArraySize = 0;

bitmap_type* PagesAlloc;
u32 InstalledMemory = 0;

#define KERNEL_START_ADDRESS 0xC0000000 // Plus Kernel Start Address

void pmm::init(multiboot_info_t *mbt) {
	MemoryDiscriptor.Addr = (multiboot_memory_map_t*)(mbt->mmap_addr + KERNEL_START_ADDRESS); 
	MemoryDiscriptor.Len = mbt->mmap_length;

	for (multiboot_memory_map_t* entry = MemoryDiscriptor.Addr; entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len;) {
		
		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));

		entry->base_addr_low += KERNEL_START_ADDRESS;
		entry->base_addr_high += KERNEL_START_ADDRESS;
		
		if (entry->base_addr_low  > 0 && (entry->length_low / (1024 * 1024)) > 0) {
			
			// Check if we alloc memory for array yet
			if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && MemoryArraySize == 0) {

				// We need to know the amount of memory we take, so we store the inital memory here
				u32 Memory = entry->base_addr_low;


				// Define the pointer for MemoryArray
				MemoryArray = (MemoryEntry*)entry->base_addr_low;
				entry->base_addr_low += sizeof(MemoryEntry) * MAX_MEMORY_ENTRY;

				// Define the pointer for PagesAlloc depending on the amount of memory the system has installed
				PagesAlloc = (bitmap_type*)entry->base_addr_low;
				PagesAlloc->addr = (u8*)(entry->base_addr_low + sizeof(bitmap_type));
				PagesAlloc->bytes = ((((mbt->mem_lower + mbt->mem_upper) * 1024) / PAGE_SIZE) / 8);
				clear_all_bits(PagesAlloc);
				
				
				entry->base_addr_low += sizeof(bitmap_type) * PagesAlloc->bytes; 

				// Calculate the differnce 
				u32 ReservedLen =  entry->base_addr_low - Memory;
				entry->length_low -= ReservedLen;

				// Print the result
				kout << "Memory Taken by Array is " << ReservedLen << "!" << endl;
			}

			MemoryArray[MemoryArraySize++] = {.MemoryAddr = entry->base_addr_low, .MemoryType = entry->type, .Len = entry->length_low};
			
			u32 Addr = MemoryArray[MemoryArraySize - 1].MemoryAddr;
			u32 AddE = MemoryArray[MemoryArraySize - 1].MemoryAddr + MemoryArray[MemoryArraySize - 1].Len;
			u32 LenS = MemoryArray[MemoryArraySize - 1].Len / (1024 * 1024);

			kout << "Found \'" << (MemoryArray[MemoryArraySize - 1].MemoryType == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED")
			  	 << "\' Memory: *" << kout.ToHex(Addr) << " "
				 << LenS << " MB [" << kout.ToHex(Addr) << ", "
				 << kout.ToHex(AddE) << "]" << endl;

			InstalledMemory += (MemoryArray[MemoryArraySize - 1].MemoryType == MULTIBOOT_MEMORY_AVAILABLE ? 1 : 0) * MemoryArray[MemoryArraySize - 1].Len;

			return;

		}
	}	

	kout << "Memory map complete!" << endl;
	
}


u32 pmm::RequestInitial(){

	return InstalledMemory;
}

u32 pmm::PagesAvailable() {
	u32 Pages = NULL;

	for (int i = 0; i < PagesAlloc->bytes * 8; i++) {
		if (!test_bit(PagesAlloc, i)) {
			Pages++;
		}
	}

	
	
	return Pages;
}

u32 pmm::ReservePage() {
	u32 bit = find_first_clear_bit(PagesAlloc);
	set_bit(PagesAlloc, bit);

	u32 ReturnAddr = 0;
	for (int i = 0; i < MemoryArraySize; i++) {
		if (MemoryArray[i].MemoryType == MULTIBOOT_MEMORY_AVAILABLE && (MemoryArray[i].MemoryAddr + (PAGE_SIZE * (bit + 1))) < MemoryArray[i].Len) {
			ReturnAddr = ((bit + 1) * PAGE_SIZE) + MemoryArray[i].MemoryAddr;
			break;
		}
	}

	memset((void*)ReturnAddr, NULL, PAGE_SIZE);

	return ReturnAddr;
}

u32 pmm::ReserveBook(u16 PagesNumber) {
	if (PagesNumber <= 0) {
		return NULL;	
	}

	u32 FirstPage = pmm::ReservePage();
	
	for (int i = 0; i < PagesNumber - 1; i++) {
		pmm::ReservePage();
	}

	return FirstPage;
}

void ForcePage(u32 offset) {
	if (test_bit(PagesAlloc, offset / PAGE_SIZE)) {
		ASSERT_NOT_REACHED("TRIED TO MAP ALREADY MAPPED ADDRESS!");
	}

	set_bit(PagesAlloc, offset / PAGE_SIZE);
}

u32 pmm::ForceBook(u16 PagesNumber, u32 offset) {
	for (int i = 0; i < PagesNumber; i++) {
		ForcePage(offset + (i * PAGE_SIZE));
	}
}

void pmm::freeBook(u32 Addr, u16 Pages) {
	for (u32 i = 0; i < Pages; i++) {
		pmm::freeBlock(Addr + (PAGE_SIZE * i));
		NO_INSTRUCTION;
	}
}

void pmm::freeBlock(u32 addr) {
	u32 bit = 0xFFFFFFFF;

	for (u32 i = 0; i < MemoryArraySize; i++) {
		if (MemoryArray[i].MemoryType == MULTIBOOT_MEMORY_AVAILABLE) {
			bit = ((addr - MemoryArray[i].MemoryAddr) / PAGE_SIZE) - 1;
			break;
		}
	}

	if (bit == 0xFFFFFFFF)
		kout << "Memory Free ERROR!! " << endl;
	else {
		clear_bit(PagesAlloc, bit);
		NO_INSTRUCTION;
	}

}



void pmm::TestMemory(System::Display::tty *tty) {
	for (int i = 0 ; i < MemoryArraySize; i ++) {
		auto m_entry = MemoryArray[i];

		if (!m_entry.Len || !m_entry.MemoryAddr)
			tty->printf("Found \'%s\' Memory: (void*)%d (Len %d MB) \n", (m_entry.MemoryType == MULTIBOOT_MEMORY_AVAILABLE ? "FREE" : "RESERVED"), m_entry.MemoryAddr == NULL ? 0 : m_entry.MemoryAddr , ((m_entry.Len == NULL ? 1 : m_entry.Len) / (1024 * 1024)));
	}		


	u32 BeforePages =  pmm::PagesAvailable();
	tty->printf("\nTotal Pages Available are %d, totaling %d MB \n",  BeforePages, (BeforePages * PAGE_SIZE) / (1024 * 1024) );

	u32 page1 = ReservePage();
	u32 page2 = ReservePage();
	kout << "Made Page!" << endl;
	freeBlock(page2);
	freeBlock(page1);
	kout << "Free Page!" << endl;
	u32 page3 = ReservePage();
	u32 page4 = ReservePage();

	u32 bookaddr = ReserveBook(100);

	u32 AfterPages = pmm::PagesAvailable();
	tty->printf("Reserved page test --> ");
	if (page1 == page3 && page2 == page4 && BeforePages - AfterPages == 102 ) {
		tty->printf("PASSED!\n");
	}
	else {
		tty->printf("FAILED!\n");
		tty->printf("%d = %d, %d = %d ; Bp - Ap = %d \n", page1, page3, page2, page4, BeforePages - AfterPages);
	}

	// Give back the memory !
	freeBlock(page3);
	freeBlock(page4);
	freeBook(bookaddr, 10);

	

	

}


