#ifndef MEMORY_HH____
#define MEMORY_HH____


#include "../core/core.h"
#include "../IO/port.h"

void memcpy(void *dest, void *src, size_t n);

int Getmemory();

void memoryInit(uint32 end); 


char* malloc(size_t size);

bool free(char* pointer);


#endif // !MEMORY_HH____
