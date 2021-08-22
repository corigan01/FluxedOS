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

#include "kmemory.hpp"
#include <System/memory/pmm/pmm.hpp>
#include <System/memory/MemorySizes.hpp>
#include <System/Display/Display.hpp>
#include <System/Clock/PIT/PIT.hpp>
#include <System/memory/paging/page.hpp>

using namespace System;
using namespace System::Memory;

#define MAX_POOL_SIZE ( 8  _MB )
#define MAX_ALLOC     (100 _KB )

void Memory::init_memory(multiboot_info_t *mbt) {
    pmm::init(mbt);
    kout << "Starting memory manger: " << pmm::PagesAvailable() << " pages available!" << endl;
}

Page_Entry Memory::map_page(Permission_Entry perm) {
    Page_Entry entry;
    entry.ptr = (void*)pmm::ReservePage();
    entry.perm = perm;
    entry.size = PAGE_S;

    kout << "Mapped page at: " << (i32)entry.ptr << " with permission: " << perm.perm << endl;

    return entry;
}
bool Memory::unmap_page(Page_Entry page) {

}

void Memory::map_page_pool(Page_Entry * page_pool, uint32_t count) {

}
void Memory::unmap_page_pool(Page_Entry * page_pool, uint32_t count) {

}

void* Memory::kmalloc(size_t size) {

}
void Memory::kfree(void* ptr) {

}