#ifndef MEMORY_HH____
#define MEMORY_HH____


#include "../core/core.h"
#include "../IO/port.h"
#include "../VGA/VGA.h"


struct KenPage {
    char* mem_start;
    char* mem_end;

    uint32 mem_size;
    
    bool inuse;

};

namespace MEMORY
{   
    constexpr uint16 PAGE = 4096;
} // namespace MEMORY


void memcpy(void *dest, void *src, size_t n);
void *memset(void *dst,char val, int n);

void SetMemory(int mem);
int Getmemory();

void memoryInit(uint32 end); 


void* malloc(uint32 size);
void * kmalloc(size_t size);

bool free(void* pointer);
bool kfree(void* pointer);

inline void  operator delete (void * p) { free(p); return; };
// /inline void* operator new    (size_t n) { return malloc(n); };


#endif // !MEMORY_HH____
