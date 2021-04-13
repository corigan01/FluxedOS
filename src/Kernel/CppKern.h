#ifndef KERN_SRT_H
#define KERN_SRT_H

#include "../lib/core/core.h"
#include "System/multibootHeader.h"


EXTNC_ int KernStart(multiboot_info_t* mbt, uint32 magic);



#endif // !KERN_SRT_H


