#include <stddef.h>
#include <stdint.h>

#ifndef KERNAL_CORE
#define KERNAL_CORE

#define KERNAL_NEEDED (short)0x00// That file is required for boot
#define KERNAL_OPTION (short)0x80// This file will not be required for boot
#define KERNAL_BACKUP (short)0xff// This will not be automaticly loaded, only loaded if a system error is detected




#endif // !KERNAL_CORE
