#include "mem.h"

void memcpy(void *dest, void *src, size_t n) 
{ 
   // Typecast src and dest addresses to (char *) 
   char *csrc = (char *)src; 
   char *cdest = (char *)dest; 
  
   // Copy contents of src[] to dest[] 
   for (int i=0; i<n; i++) 
       cdest[i] = csrc[i]; 
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