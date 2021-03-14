#ifndef MEMORY_HH____
#define MEMORY_HH____


#include "../core/core.h"
#include "../IO/port.h"

void memcpy(void *dest, void *src, size_t n);
void *memset(void *dst,char val, int n);

int Getmemory();

void memoryInit(uint32 end); 


void* malloc(uint32 size);

bool free(void* pointer);


#endif // !MEMORY_HH____
