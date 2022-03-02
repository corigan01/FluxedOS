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

#include <System/memory/vmm/vmm.hpp>
#include <System/kout/kout.hpp>
#include <System/memory/kmemory.hpp>

using namespace System;
using namespace System::Memory;
using namespace System::Memory::vmm;

#define ERASE_PERMS(addr)   (((u32)addr) & 0xFFFFF000)
#define TABLE(pointer)      ((u32*)(ERASE_PERMS(pointer)))
#define TO_POINTER(addr)    ((u32*)((u32)addr))
#define TABLE_SIZE          1024
#define AT(dir)             [dir]
#define IS_PRESENT(addr)    ((addr & 0x00000001) == PRESENT) 
#define MAX(a, b)           ((a > b) ? a : b)


// The system only has one page directory and this is it
// This is our system page directory, but what is cool about this
// is that it stores both the phys and virt address of the tables
// so we can still access them while being able to submit it to the MMU
vmm::page_directory_t SystemDirectory;

// this will construct a new entry into our directory
void vmm::new_page_table(vmm::page_directory_t dir, u32 index, u32 perms) {
    // Ask skmalloc for a space to store the table
    vmm::page_table_t new_table = (vmm::page_table_t)PAGE_ALIGN(StaticHeap::skmalloc(8 _KB));

    // Then tell Page that we would like to make a new table
    Page::SendTableToDirectory((u32)dir.table, (u32)dir.vtable, (u32)new_table, index, perms);
}

// This will ask pmm for some memory and then map that memory to our page table
void vmm::pmm_table_fill(vmm::page_directory_t dir, u32 index, u32 perms) {
    // Make sure we have a table
    if (dir.vtable[index] <= 0) new_page_table(dir, index, perms);

    // Get the table so we can use it
    vmm::page_table_t table = TO_POINTER( ERASE_PERMS (dir.vtable[index]) ); 

    // loop through the entries and map whatever pmm gives us as a phys address
    for (int i = 0; i < 1024; i ++) {    
        table[i] = ((pmm::ReservePage()) & 0xFFFFF000) | perms;
    }

    // finally make sure we flush the pages so they take affect
    Page::SendTableToDirectory((u32)dir.table, (u32)dir.vtable, (u32)table, index, perms);
}

// This will map phy to the table
// BE CAREFULE BECAUSE THE MEMORY NEEDS TO EXIST
void vmm::manual_table_fill(vmm::page_directory_t dir, u32 index, u32 phy, u32 size, u32 perms) {
    // Make sure we have a table
    if (dir.vtable[index] <= 0) new_page_table(dir, index, perms);

    kout << "PHY: " << kout.ToHex(phy) << "  --> " << size << endl;

    // Get the table so we can use it
    vmm::page_table_t table = TO_POINTER( ERASE_PERMS (dir.vtable[index]) ); 

    // loop through the entries and map whatever pmm gives us as a phys address
    for (int i = 0; i < (size / 4096); i ++) {    
        table[i] = ((phy & 0xFFFFF000) + (i * (4 _KB))) | perms;
    }

    for (int i = (size / 4096); i < 1024; i ++) {    
        table[i] = NULL;
    }

    // finally make sure we flush the pages so they take affect
    Page::SendTableToDirectory((u32)dir.table, (u32)dir.vtable, (u32)table, index, perms);
}

// Grab the SystemDirectory
vmm::page_directory_t vmm::GetSystemDirectory() {
    return SystemDirectory;
}

void vmm::init(multiboot_info_t* mbt, u32 boot_page_dir) {
    // First we need to look at where the kernel booted from.
    // This will be useful because we need to know where skmalloc
    // points to. 
    page_table_t KernelTable = TO_POINTER(ERASE_PERMS( TABLE(boot_page_dir)[PAGEDIR_INDEX(LOAD_MEMORY_ADDRESS)] ));
    KernelTable = TO_POINTER(((u32)KernelTable) + LOAD_MEMORY_ADDRESS);

    // This is where we will look through the kernel table and find where everything is!
    // So we need to look through each entry and we know when we reached the end of that
    // alloc is when there is a gap. This means we just need to look for the gaps.
    // Usally the map looks as follows
    // [1]     ->  [~300]    KERNEL     
    // [~300]  ->  [???]     FREE SPACE 
    // [???]   ->  [1022]    SKMALLOC
    // [1023]                FREE SPACE 
    // [1024]                N/A

    bool ClearedKernel  = 0; // This is true when we reached the end of where the kernel is stored
    u32 KernelEnd       = 0; // This is the highest memory address in the entire table
    u32 SKMallocStart   = 0; // This is where skmalloc starts 
    u32 SKMallocEnd     = 0; // This is where skmalloc ends (again it ends at another gap) 

    for (int i = 0; i < TABLE_SIZE; i++) {
        u32 table_entry = KernelTable[i];

        if (table_entry > 0) {
            KernelEnd = MAX(table_entry, KernelEnd);

            if (ClearedKernel) {
                if (!SKMallocStart) SKMallocStart = PAGETBL_ADDRESS(i) + LOAD_MEMORY_ADDRESS;
                else                SKMallocEnd   = PAGETBL_ADDRESS(i) + LOAD_MEMORY_ADDRESS;
            }
        }
        else {
            if (ClearedKernel && SKMallocStart)   break;
            else                                  ClearedKernel    = true;
        }
    }

    // Now we have all the needed information to give skmalloc some memory
    StaticHeap::init((void*)SKMallocStart, (SKMallocEnd - SKMallocStart));

    // After we get Skmalloc setup, its time to get the physical memory map
    // the multiboot header provides a detailed map of which memory maps we can
    // use and not use, so the pmm takes care of giving us that
    pmm::init(mbt);

    // The pmm does not do one thing though, it now needs to know where the kernel stops
    // So we need to manually alloc those pages away. We can figure out how many
    // pages to let go, by seeing when the pmm starts and looping up until `KernelEnd`
    for (int i = pmm::ReservePage(); i < KernelEnd; i += PAGE_SIZE) {
        pmm::ReservePage(); 
    }

    // As we now have a clear memory array, we can begin setting up paging
    // The paging manager is kinda dumb, however, it can only map phy to virtual
    // so thats why we need the vmm (Virtual Memory Manager). But this namespace
    // feels much more like an ALL Memory Manager :)
    Page::init(boot_page_dir);

    // Now lets start storing some of our own infomation!
    // We are going to first need the Global Page Directory
    SystemDirectory.table = (u32*)Page::GetPageDir();

    // Now we need to setup virtual table
    SystemDirectory.vtable = (page_table_t)PAGE_ALIGN(StaticHeap::skmalloc(8 _KB));
    SystemDirectory.vtable[PAGEDIR_INDEX(LOAD_MEMORY_ADDRESS)] = SystemDirectory.table[PAGEDIR_INDEX(LOAD_MEMORY_ADDRESS)] + LOAD_MEMORY_ADDRESS;

    // Now we need to get some more memory before our ~40KB runs out
    // So we map an entire page table to skmalloc
    pmm_table_fill(SystemDirectory, 769, SUPERVISOR | PRESENT | R_W);
    
    // Now give that memory to skmalloc
    StaticHeap::init((void*)PAGEDIR_ADDRESS(769), 4 _MB);

    // So now that we have around ~4MB of memory in skmalloc we can begin kheap
    for (int i = 0; i < ((KHEAP_MAX_POOL_SIZE) / (4 _MB)); i++) {
        pmm_table_fill(SystemDirectory, 770 + i, SUPERVISOR | PRESENT | R_W);
    }

    // Now give that memory to kheap
    Memory::init_memory(mbt, 770, 770 + (KHEAP_MAX_POOL_SIZE / (4 _MB)));
    

    kout << kout.BOLD_GREEN << "VMM DONE!" << kout.YELLOW << endl;

}

 