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
#include <System/memory/staticalloc/skmalloc.hpp>

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

	u32 MemoryEntries = 0;
	
	// First we need to calc how many entries we have, so then we can ask
	// skmalloc nicely how much we need to store!
	for (multiboot_memory_map_t* entry = MemoryDiscriptor.Addr; entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len;) {
		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));
		if (entry->base_addr_low <= 0) continue;

		MemoryEntries++;
	}	

	// Now we know how many entries we have, so we can acually store them
	MemoryArray = (MemoryEntry*)StaticHeap::skmalloc(MemoryEntries * sizeof(MemoryEntry));

	// As we now have a place for MemoryArray to live, We can iterate over 
	// the Memory again and acually store it this time :) 
	{
		kout << "\nPhysical Memory Space:" << endl;
		kout << "\t|    ADDR    |    BYTES   |    SIZE     |  TYPE |" << endl;
		kout << "\t|------------|------------|-------------|-------|" << endl;

		// This makes sure `i` goes out of scope
		u32 i = 0;
		for (multiboot_memory_map_t* entry = MemoryDiscriptor.Addr; entry < MemoryDiscriptor.Addr + MemoryDiscriptor.Len; i++) {
				
			entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));
			if (entry->base_addr_low <= 0) continue;
			
			kout << ((entry->type == MULTIBOOT_MEMORY_AVAILABLE) ? kout.GREEN : kout.YELLOW) << "\t| 0x" << kout.ToHex(entry->base_addr_low) << 
			" | 0x" << kout.ToHex(entry->length_low) << " | " << entry->length_low / (1 _MB) << "MB  \t| "
			<< (entry->type == 1 ? kout.GREEN : kout.RED) << (entry->type == 1 ? "FREE" : "RESV") << kout.YELLOW << "\t|" << kout.YELLOW << endl; 

			MemoryArray[i].MemoryType = entry->type;
			MemoryArray[i].MemoryAddr = entry->base_addr_low;
			MemoryArray[i].Len        = entry->length_low;
			MemoryArraySize++;
		}

		kout << "\t|------------|------------|-------------|-------|" << endl << endl;
	}

	// Now we need to calc the total size of the memory we have
	// This is important because we need to know how much to
	// store in our bitmap
	for (int i = 0; i < MemoryArraySize; i++) {
		if (MemoryArray[i].MemoryType != MULTIBOOT_MEMORY_AVAILABLE) continue;

		InstalledMemory += MemoryArray[i].Len;
	}

	// This is how many bytes we have to store 
	u32 PagesAllocSize = (InstalledMemory / (4 _KB)) / 8;

	// Now lets alloc this
	PagesAlloc = (bitmap_type*)StaticHeap::skmalloc(sizeof(bitmap_type));
	PagesAlloc->addr = (u8*)StaticHeap::skmalloc(PagesAllocSize);
	PagesAlloc->bytes = PagesAllocSize;

	// Make sure all bytes are 0 because its a bit map and we need to make sure its clear
	memset((void*)PagesAlloc->addr, 0x0, PagesAllocSize);
	
}

// This is how much memory we had at the start (installed ram)
u32 pmm::RequestInitial(){
	return InstalledMemory;
}

// This is how many pages we currently have that are not mapped
u32 pmm::PagesAvailable() {
	u32 Pages = 0;

	// Locate all clear bits and add to Pages
	for (int i = 0; i < PagesAlloc->bytes * 8; i++) {
		if (!test_bit(PagesAlloc, i)) {
			Pages++;
		}
	}
	
	return Pages;
}

// This will give you one page alined region of memory
u32 pmm::ReservePage() {
	// Finds a bit that is clear
	u32 bit = find_first_clear_bit(PagesAlloc);

	// Sets that bit because its now in use
	set_bit(PagesAlloc, bit);

	// Now we need to know where that bit points to.
	// So we begin by finding the raw addr, this will be
	// added to the start of the memory region we have
	u32 CalculatedBitAddr = bit * PAGE_SIZE;
	u32 OffsetAddress = 0;

	// Find the clear memory buffer and set offset to it
	for (int i = 0; i < MemoryArraySize; i++) {
		if (MemoryArray[i].MemoryType == MULTIBOOT_MEMORY_AVAILABLE) {
			OffsetAddress = MemoryArray[i].MemoryAddr;
		}
	}

	// Finally add the two together so we have our addr
	u32 ReturnAddr = CalculatedBitAddr + OffsetAddress;

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

	return 0;
}

void pmm::freeBook(u32 Addr, u16 Pages) {
	for (u32 i = 0; i < Pages; i++) {
		pmm::freeBlock(Addr + (PAGE_SIZE * i));
		//NO_INSTRUCTION;
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
		//NO_INSTRUCTION;
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


