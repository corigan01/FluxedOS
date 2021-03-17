#ifndef DRIVE_KERNEL__H_
#define DRIVE_KERNEL__H_

#include "../lib/core/core.h"
#include "../lib/IO/port.h"
#include "../lib/IO/ATA/ata.h"
#include "../lib/vfs/vfs.h"
#include "../lib/VGA/VGA.h"
#include "fs/ext2.h"

// This lib is going to switch drives depending on where they come from.
// Allowing the Kernel to not care on which interface the drive comes from. 
// This only supports ATA (IDE) Drives at the moment, but will be expanded soon

namespace DISK {
    
	


};



#endif // !DRIVE_KERNEL__H_