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



#define PAGE_S 0x400000

namespace System
{
    namespace Memory
    {
        namespace Page
        {
            
            void init();
            void switch_page(i32* page_dir);
            void enable_paging();

            void map_addr(i32 v, i32 p);
            void map_page(i32* page_dir, i32 vpage, i32 ppage);

            void id_map(i32 PStart, i32 VStart, i32 Offset);
            void dump_page(i32* page);

            i32* mk_page();
            i32* mk_page_dir();

            i32* RootDir();

            typedef struct{
                i32 present	: 1;
                i32 rw		: 1;
                i32 user	: 1;
                i32 accessed: 1;
                i32 dirty	: 1;
                i32 unused	: 7;
                i32 frame	: 20;
            } page_t;

            typedef struct{
                page_t pages[1024];
            } page_table_t;

            typedef struct{
                page_table_t* tables[1024];
                i32 tables_physical[1024];
                i32 physical_address;
            } page_directory_t;


        } // namespace Page
        
    } // namespace Memory
    
}