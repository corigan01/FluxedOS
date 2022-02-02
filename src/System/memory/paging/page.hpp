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

            typedef struct{
                u32 present	  : 1;
                u32 rw		  : 1;
                u32 user	  : 1;
                u32 accessed  : 1;
                u32 dirty	  : 1;
                u32 unused	  : 7;
                u32 frame	  : 20;
            } page_t;

            typedef struct{
                page_t pages[1024];
            } page_table_t;

            typedef struct{
                page_table_t* tables[1024];
                u32 tables_physical[1024];
                u32 physical_address;
            } page_directory_t;


            
            void init();

            page_directory_t construct_page_directory();
            page_table_t construct_page_table();
            
            void map_page(page_directory_t* dir, u32 virtual_address, u32 physical_address, u32 flags);
            void map_page_table(page_directory_t* dir, u32 virtual_address, page_table_t* table, u32 flags);
            void map_page_directory(page_directory_t* dir, u32 virtual_address, page_directory_t* table, u32 flags);

            void unmap_page(page_directory_t* dir, u32 virtual_address);
            void unmap_page_table(page_directory_t* dir, u32 virtual_address);
            void unmap_page_directory(page_directory_t* dir, u32 virtual_address);

            void enable_paging(page_directory_t* page_directory);



        } // namespace Page
        
    } // namespace Memory
    
}
