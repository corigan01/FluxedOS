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

#pragma once

#include "../../../lib/core/core.h"
#include "../../Serial/serial.hpp"
#include <System/memory/MemorySizes.hpp>

// Bit 0 (P) is the Present flag.
// Bit 1 (R/W) is the Read/Write flag.
// Bit 2 (U/S) is the User/Supervisor flag.

// TODO: Define Fault stuff

#define PRESENT_FLAG        0b001
#define READ_WRITE_ENABLED  0b010
#define SUPER_USER_MEMORY   0b000
#define USER_SPACE_MEMORY   0b100

#define PAGE_S 0x400000

namespace System
{
    namespace Memory
    {
        namespace Page
        {

            #define PAGE_SIZE 4096

            // Alignment related macro
            #define IS_ALIGN(addr) ((((u32)(addr)) & 0x00000FFF) == 0)
            #define PAGE_ALIGN(addr) ((((u32)(addr)) & 0xFFFFF000) + 0x1000)

            // Defone some address calculation macro
            #define PAGEDIR_INDEX(vaddr) (((u32)vaddr) >> 22)
            #define PAGETBL_INDEX(vaddr) ((((u32)vaddr) >>12) & 0x3ff)
            #define PAGEFRAME_INDEX(vaddr) (((u32)vaddr) & 0xfff)

            // Paging register manipulation macro
            #define SET_PGBIT(cr0) (cr0 = cr0 | 0x80000000)
            #define CLEAR_PSEBIT(cr4) (cr4 = cr4 & 0xffffffef)

            

            #define LOAD_MEMORY_ADDRESS 0xC0000000


            
            void init(u32 bpg);


        } // namespace Page
        
    } // namespace Memory
    
}
