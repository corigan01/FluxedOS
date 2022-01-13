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

#include "bitmap.hpp"
#include "../StanderdOperations/Operations.hpp"


bool test_bit(bitmap_type *bitmap, u32 bit)
{
	return (bool)((bitmap->addr[bit / 8] & (0x1 << (bit % 8))) != 0);
}

void set_bit(bitmap_type *bitmap, u32 bit)
{
	bitmap->addr[bit / 8] |= (0x1 << (bit % 8));
}

void clear_bit(bitmap_type *bitmap, u32 bit)
{
	bitmap->addr[bit / 8] &= ~(0x1 << (bit % 8));
}

u32 find_first_set_bit(bitmap_type *bitmap)
{
	for (u32 i = 0; i < bitmap->bytes; i++) {
		if (bitmap->addr[i] != 0x0) {
			for (u8 j = 0; j < 8; j++) {
				if ((bitmap->addr[i] & (0x1 << j))) {
					return ((i * 8) + j);
				}
			}
		}
	}
	return 0;
}

u32 find_first_clear_bit(bitmap_type *bitmap)
{
	for (u32 i = 0; i < bitmap->bytes; i++) {
		if (bitmap->addr[i] != 0xFF) {
			for (u8 j = 0; j < 8; j++) {
				if (!(bitmap->addr[i] & (0x1 << j))) {
					return ((i * 8) + j);
				}
			}
		}
	}
	return 0;
}

void clear_all_bits(bitmap_type *bitmap)
{
	memset((u8 *) bitmap->addr, 0x0, bitmap->bytes);
}

void set_all_bits(bitmap_type *bitmap)
{
	memset((u8 *) bitmap->addr, 0xFF, bitmap->bytes);
}

bool any_bit_clear(bitmap_type *bitmap)
{
	for (u32 i = 0; i < bitmap->bytes; i++) {
		if (bitmap->addr[i] != 0xFF) {
			return true;
		}
	}
	return false;
}

bool any_bit_set(bitmap_type *bitmap)
{
	for (u32 i = 0; i < bitmap->bytes; i++) {
		if (bitmap->addr[i] != 0x0) {
			return true;
		}
	}
	return false;
}
