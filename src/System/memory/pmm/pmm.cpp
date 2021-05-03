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
#include "../../kout/kout.hpp"
#include "../../Power/Power.hpp"
#include "../../../lib/bitmap/bitmap.hpp"

using namespace System;
using namespace System::Memory;

extern i32 start;
extern i32 end;

i32 boot_page_dir_addr =  0 ;
i32 kernel_start = (i32) &start;
i32 kernel_end = (i32) &end;

bitmap_type *pmm_frames = 0x0;

void pmm::init(multiboot_info_t *mboot_ptr) {
	
    if ((i32) pmm_frames != 0x0) {
		return;
	}
	
	i32 mem_in_mb = mboot_ptr->mem_upper / 1024 + 2;
	i32 mem_in_kb = mem_in_mb * 1024;
	i32 num_frames = mem_in_kb / 4;
	
	
	i32 frames_addr = ((kernel_end & ~(0xFFF)) + 0x1000) - sizeof(bitmap_type);
	i32 bitmap_addr = ((kernel_end & ~(0xFFF)) + 0x1000);
	i32 bitmap_size = num_frames / 8;
	
	
	pmm_frames = (bitmap_type *) frames_addr;
	pmm_frames->addr = (i8 *) bitmap_addr;
	pmm_frames->bytes = bitmap_size;
	
	set_all_bits(pmm_frames);
	
	multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) (mboot_ptr->mmap_addr + 0xC0000000);
	
	while ((i32) mmap < mboot_ptr->mmap_addr + 0xC0000000 + mboot_ptr->mmap_length) {

		if (mmap->type == 0x1) {

			i32 region_start = mmap->addr;
			i32 region_frames = mmap->len / 0x1000;
			
			for (i32 i = 0; i < region_frames; i++) {
				i32 four_k_frame_addr = region_start + i * 0x1000;
				i32 bit_on_bitmap = four_k_frame_addr / 0x1000;
		

				clear_bit(pmm_frames, bit_on_bitmap);
			}
		}
		
		mmap = (multiboot_memory_map_t *) ((i32) mmap + mmap->size + sizeof(i32));
	}
	
	i32 *boot_page_dir = (i32 *) boot_page_dir_addr;
	
	for (i32 i = 0; i < 1024; i++) {
		if (boot_page_dir[i] & 0x1) {
			i32 phys_addr = boot_page_dir[i] & ~(0xFFF);
			
			for (i32 j = 0; j < 1024; j++)
			{
				i32 four_k_frame_addr = j * 0x1000 + phys_addr;
				i32 bit_on_bitmap = four_k_frame_addr / 0x1000;
				set_bit(pmm_frames, bit_on_bitmap);
			}
		}
	}
}

i32 pmm::alloc_frame() {
	if ((i32) pmm_frames == 0x0) {
		kout << "Physical memory manager has not been initialized!" << endl;
		kout << "Halting" << endl;
		
        Power::hold();
	}
	
	i32 result = 0xFFFFFFFF;
	
	if (any_bit_clear(pmm_frames)) {

		i32 first_clear_bit = find_first_clear_bit(pmm_frames);
		set_bit(pmm_frames, first_clear_bit);
		i32 frame_addr = first_clear_bit * 0x1000;

		result = frame_addr;
	}
	
	return result;
}

void pmm::free_frame(i32 addr) {
	if ((i32) pmm_frames == 0x0)
	{
		kout << "Physical memory manager has not been initialized!" << endl;
		kout << "Halting" << endl;

		Power::hold();
	}
	
	i32 aligned_addr = addr & ~(0xFFF);
	i32 bit_on_bitmap = aligned_addr / 0x1000;
	clear_bit(pmm_frames, bit_on_bitmap);
	
}
