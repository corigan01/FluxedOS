#ifndef MEMORY_HH____
#define MEMORY_HH____


#include "../core/core.h"
#include "../IO/port.h"

void memcpy(void *dest, void *src, size_t n);
void *memset(void *dst,char val, int n);

int Getmemory();
void SetMem(unsigned int memory);

void memoryInit(uint32 end); 


void* malloc(uint32 size);

bool free(void* pointer);

inline void  operator delete (void * p) { free(p); return; };


// /inline void* operator new    (size_t n) { return malloc(n); };


#endif // !MEMORY_HH____
