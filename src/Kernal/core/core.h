#ifndef KERNAL_CORE
#define KERNAL_CORE

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t size_t;


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;



#define NULL 0x00


#define KERNAL_NEEDED (short)0x00// That file is required for boot
#define KERNAL_OPTION (short)0x80// This file will not be required for boot
#define KERNAL_BACKUP (short)0xff// This will not be automaticly loaded, only loaded if a system error is detected

#define Kernal_main kernel_entry



#endif // !KERNAL_CORE
