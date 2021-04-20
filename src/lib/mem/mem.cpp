#include "mem.h"
#include "../Vector/vector.h"


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

uint32 GlobalMemory = 0;
void SetMemory(int mem) {
    GlobalMemory = mem;
}

int Getmemory() {
    return GlobalMemory;
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
    KenHeap.Start       = end + (VECTOR_MAX_SIZE * sizeof(KenPage));
    KenHeap.locPointer  = (MemoryReserve*)KenHeap.Start + 10;
    KenHeap.TotalMemory = ((Getmemory() * 1024) - ((20 * 1024) + KenHeap.Start)); // Take off 10MB for padding, and add where the kernel is loaded into memory. 
    KenHeap.UsedMemory  = 0;
    KenHeap.UsedBlocks  = 0;
    KenHeap.EnableMemoryloc = 1;

    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
    VGA::PRINT_STR("MEM INIT ");
    VGA::SET_COLOR(VGA::COLORS::GREEN, VGA::COLORS::BLACK);
    VGA::PRINT_STR("OK -- ");
    VGA::PRINT_INT(KenHeap.Start);
    VGA::PRINT_STR("KB free\n");
    VGA::SET_COLOR(VGA::COLORS::WHITE, VGA::COLORS::BLACK);
}


void* malloc(uint32 size) {
    //VGA::PRINT_STR("S: ");
    //VGA::PRINT_INT(size);
    //VGA::PRINT_STR(" ");
    if (!KenHeap.EnableMemoryloc) {
        //panic("Tried to loc memory, when memory is locked!");
    }

    if (size == 0) {
        //panic("Tried to loc 0 bytes of memory!");
        //VGA::PRINT_STR("Tried to loc 0 bytes of memory!\n");
        return (char*)-1;
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

                

                memset((void*)pice->start_address, NULL, pice->used);

                allo = 1;
            }
            else if (pice->used > size && !allo) {
                pice->inuse = 1;
                ReturnAddr = pice->start_address + 4;
                pice->end_address = pice->start_address + size + 8;

                pice->used = pice->end_address - pice->start_address;

                

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

       

    }
    else if (!allo) {
        //G_ERR(VGA::PRINT_STR("MEMORY ERR ["));
        //G_ERR(VGA::PRINT_INT((KenHeap.TotalMemory - KenHeap.UsedMemory) - allocEnd));
        //G_ERR(VGA::PRINT_STR(", "));
        //G_ERR(VGA::PRINT_INT(size));
        //G_ERR(VGA::PRINT_STR("]\n"));

        return (void*)-1;
        //ThrowISR(19); // Throw "Out of Memory"
    }

    



    return (char*)(ReturnAddr); 
}

Vector<KenPage> kenH(KenHeap.Start - (VECTOR_MAX_SIZE * sizeof(KenPage)));
void * kmalloc(size_t size) {
   
    void * memory_alloc = 0;

    for (int i = 0; i < kenH.size(); i++) {
        auto &e = kenH[i];

        if (!e.inuse) {
            if (e.mem_size == size) {
                e.inuse = true;
                memory_alloc = (void*)e.mem_start;

                VGA::kprintf("Found unused memory with exact size \n     addr: %d \n", memory_alloc);
                
                return memory_alloc;
            }
            else if (e.mem_size > size) {                
                KenPage d = {
                    e.mem_start + size + 1,
                    e.mem_end,
                    (e.mem_end - (e.mem_start + size + 1)),
                    false 
                };
                
                VGA::kprintf("mush shrink size of chunk %d to new %d and %d \n     addr: %d \n", e.mem_size, size, d.mem_size, memory_alloc);

                kenH.insert_at(i + 1, d);
                
                e.mem_end = size + e.mem_start;
                e.mem_size = size;
                e.inuse = true;

                memory_alloc = (void*)e.mem_start;

                return memory_alloc;
            }

        }
    }

    char* LastMemoryAddr = (char*)KenHeap.Start + 1;
    if (kenH.size() > 0) {
        LastMemoryAddr = (char*)(kenH[ kenH.size() - 1 ].mem_end) + 1U;
    }
    
    KenPage d = {
        LastMemoryAddr,
        LastMemoryAddr + size,
        size,
        true
    };

    
    

    VGA::kprintf("Making new memory chunk with size %d \n     addr: %d \n", size, d.mem_end);

    kenH.push_back(d);
    return LastMemoryAddr;
}

bool kfree(void* pointer) {
    for (int i = 0; i < kenH.size(); i++) {
        auto e = kenH[i];

        if (e.mem_start == pointer) {
            kenH[i].inuse =  false;
        }
    }
}

bool free(void* pointer) {
    for (int i = 0; i < KenHeap.UsedBlocks; i++) {
        auto pice = KenHeap.locPointer + i;
        if (((uint32)pointer - 4) == pice->start_address) {
            

            pice->inuse = 0;
            
            // write all NULL to data for de-alloc
            for (uint32 e = pice->start_address; i < pice->end_address; i++) {
                char* h = (char*)(e);

                h = (char*)NULL;
            }

            return 1;
        }
    }

    //G_ERR(VGA::PRINT_STR("Could not find that memory allocated!\n"));
    return 0;
}