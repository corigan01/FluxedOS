#ifndef KERNAL_CORE
#define KERNAL_CORE

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t size_t;


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


#define NULL (char)0x00


#define KERNAL_NEEDED (int)0x00// That file is required for boot
#define KERNAL_OPTION (int)0x80// This file will not be required for boot
#define KERNAL_BACKUP (int)0xff// This will not be automaticly loaded, only loaded if a system error is detected



#ifdef __cplusplus
#define EXTNC_ extern "C"
#else
#define EXTNC_
#endif

#define EXTNC_START {
#define ECTNC_END }





#endif // !KERNAL_CORE
