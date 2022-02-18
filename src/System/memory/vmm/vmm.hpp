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

#include <lib/core/core.h>
#include <System/memory/pmm/pmm.hpp>
#include <System/memory/paging/page.hpp>
#include <lib/multiboot/multiboot.h>
#include <System/memory/staticalloc/skmalloc.hpp>

namespace System {
    namespace Memory {
        namespace vmm {

            #define PAGE_SIZE 4096

            #define SUPERVISOR  0b000
            #define USER        0b100
            #define R_W         0b010
            #define READ_ONLY   0b000
            #define PRESENT     0b001

            #define KHEAP_MAX_POOL_SIZE ( 40 _MB )
            #define KHEAP_MAX_ALLOC     ( 8  _MB )
 
            
            typedef u32* page_table_t;
            typedef struct {
                page_table_t table;
                page_table_t vtable;
            } page_directory_t;


            void init(multiboot_info_t* mbt, u32 boot_page_dir);

            void* AllocPage(u32 perms);

            // this will construct a new entry into our directory
            void new_page_table(vmm::page_directory_t dir, u32 index, u32 perms);

            // This will ask pmm for some memory and then map that memory to our page table
            void pmm_table_fill(vmm::page_directory_t dir, u32 index, u32 perms);

            // This will map phy to the table
            // BE CAREFULE BECAUSE THIS MAPS 4MB of WHATEVER PHYS YOU GIVE IT 
            void manual_table_fill(vmm::page_directory_t dir, u32 index, u32 phy, u32 perms);

            // Grab the SystemDirectory
            vmm::page_directory_t GetSystemDirectory();
            

            
        }
    }
}