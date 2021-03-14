#include "mem.h"
#include "../VGA/VGA.h"



void memcpy(void *dest, void *src, size_t n) 
{ 
   // Typecast src and dest addresses to (char *) 
   char *csrc = (char *)src; 
   char *cdest = (char *)dest; 
  
   // Copy contents of src[] to dest[] 
   for (int i=0; i<n; i++) 
       cdest[i] = csrc[i]; 
} 

void *memset(void *dst,char val, int n)
{
    char *temp = (char*)dst;
    for(;n != 0; n--) *temp++ = val;
    return dst;
}

int Getmemory() {
    unsigned int total;
    unsigned int lowmem, highmem;
 
    port_byte_out(0x70, 0x30);
    lowmem = port_byte_in(0x71);
    port_byte_out(0x70, 0x31);
    highmem = port_byte_in(0x71);
 
    total = lowmem | highmem << 8;
    return total;
}

struct MemoryReserve {
    uint32 start_address = 0; // begining address
    uint32 end_address   = 0; // end address
    uint32 used          = 0; // used memory

    bool inuse           = 0; // inuse
};
struct KernelHeap {
    uint32 Start = 0;
    uint32 TotalMemory = 0;
    uint32 UsedMemory  = 0;

    bool EnableMemoryloc = 1;

    MemoryReserve *locPointer;
    uint32 UsedBlocks  = 0;

} KenHeap;

void memoryInit(uint32 end) {
    KenHeap.Start       = end;
    KenHeap.locPointer  = (MemoryReserve*)KenHeap.Start + 10;
    KenHeap.TotalMemory = ((Getmemory() * 1024) - ((20 * 1024) + KenHeap.Start)); // Take off 10MB for padding, and add where the kernel is loaded into memory. 
    KenHeap.UsedMemory  = 0;
    KenHeap.UsedBlocks  = 0;
    KenHeap.EnableMemoryloc = 1;

    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
    VGA::PRINT_STR("MEM INIT ");
    VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
    VGA::PRINT_STR("OK -- ");
    VGA::PRINT_INT(KenHeap.TotalMemory / 1024);
    VGA::PRINT_STR("KB free\n");
    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
}


void* malloc(uint32 size) {
    //VGA::PRINT_STR("S: ");
    //VGA::PRINT_INT(size);
    //VGA::PRINT_STR(" ");
    if (!KenHeap.EnableMemoryloc) {
        panic("Tried to loc memory, when memory is locked!");
    }

    if (size == 0) {
        //panic("Tried to loc 0 bytes of memory!");
        VGA::PRINT_STR("Tried to loc 0 bytes of memory!\n");
        return (char*)-1;
    }

    else if ( (KenHeap.TotalMemory - KenHeap.UsedMemory) < size) {
        ThrowISR(19); // Throw "Out of Memory"
        //panic("Kernel is out of memory!");

    }

    bool allo = 0;

    uint32 ReturnAddr = -1;

    uint32 allocEnd  = 0;
    uint32 LastAlloc = 0;
    for (int i = 0 ; i < KenHeap.UsedBlocks; i++) {
        auto pice = KenHeap.locPointer + i;


        if (!pice->inuse) {
            if (pice->used == size && !allo){
                pice->inuse = 1;
                ReturnAddr = pice->start_address + 4;

                VGA::PRINT_STR("[EXISTING] alloc (");
                VGA::PRINT_INT(pice->start_address);
                VGA::PRINT_STR(", ");
                VGA::PRINT_INT(pice->end_address);
                VGA::PRINT_STR(") [");
                VGA::PRINT_INT(pice->used);
                VGA::PRINT_STR("][");
                VGA::PRINT_INT(size);
                VGA::PRINT_STR("] Bytes!\n");

                memset((void*)pice->start_address, NULL, pice->used);

                allo = 1;
            }
            else if (pice->used > size && !allo) {
                pice->inuse = 1;
                ReturnAddr = pice->start_address + 4;
                pice->end_address = pice->start_address + size + 8;

                pice->used = pice->end_address - pice->start_address;

                VGA::PRINT_STR("[RESIZE] alloc (");
                VGA::PRINT_INT(pice->start_address);
                VGA::PRINT_STR(", ");
                VGA::PRINT_INT(pice->end_address);
                VGA::PRINT_STR(") [");
                VGA::PRINT_INT(pice->used);
                VGA::PRINT_STR("][");
                VGA::PRINT_INT(size);
                VGA::PRINT_STR("] Bytes!\n");

                memset((void*)pice->start_address, NULL, pice->used);

                allo = 1;
            }
            else {
                ; // memory adv is smaller then we need loc
            }
        }

        // adding to loc end
        if (LastAlloc > pice->end_address) 
            allocEnd = LastAlloc;
        else 
            allocEnd = pice->end_address;
    }

    if ((KenHeap.TotalMemory - KenHeap.UsedMemory) - allocEnd >= size && !allo) {
        auto pice = KenHeap.locPointer + KenHeap.UsedBlocks;

        pice->inuse         = 1;
        pice->start_address = KenHeap.UsedBlocks + allocEnd;
        pice->end_address   = pice->start_address + size + 8;
        pice->used          = pice->end_address - pice->start_address;

    
        ReturnAddr = pice->start_address + 4;

        memset((void*)pice->start_address, NULL, pice->used);

        KenHeap.UsedBlocks++;

        /*VGA::PRINT_STR("[NEW] alloc (");
        VGA::PRINT_INT(pice->start_address);
        VGA::PRINT_STR(", ");
        VGA::PRINT_INT(pice->end_address);
        VGA::PRINT_STR(") [");
        VGA::PRINT_INT(pice->used);
        VGA::PRINT_STR("] Bytes!\n");*/

    }
    else if (!allo) {
         ThrowISR(19); // Throw "Out of Memory"
    }

    



    return (char*)(ReturnAddr); 
}

bool free(void* pointer) {
    for (int i = 0; i < KenHeap.UsedBlocks; i++) {
        auto pice = KenHeap.locPointer + i;
        if (((uint32)pointer - 4) == pice->start_address) {
            /*VGA::PRINT_STR("[REMOVED] alloc (");
            VGA::PRINT_INT(pice->start_address);
            VGA::PRINT_STR(", ");
            VGA::PRINT_INT(pice->end_address);
            VGA::PRINT_STR(") [");
            VGA::PRINT_INT(pice->used);
            VGA::PRINT_STR("] Bytes!\n");*/

            pice->inuse = 0;
            
            // write all NULL to data for de-alloc
            for (uint32 e = pice->start_address; i < pice->end_address; i++) {
                char* h = (char*)(e);

                h = (char*)NULL;
            }

            return 1;
        }
    }

    VGA::PRINT_STR("Could not find that memory allocated!");
    return 0;
}