#ifndef KERNAL_CORE
#define KERNAL_CORE

// info magic
#define MB_INFO_MEM                             0x00000001
#define MB_INFO_BOOTDEV                         0x00000002
#define MB_INFO_CMDLINE                         0x00000004
#define MB_INFO_MOD                             0x00000008
#define MB_INFO_AOUT_SYMS                       0x00000010
#define MB_INFO_ELF                             0X00000020
#define MB_INFO_MEMMAP                          0x00000040
#define MB_INFO_DRIVE                           0x00000080
#define MB_INFO_CONF_TABLE                      0x00000100
#define MB_INFO_BL_NAME                         0x00000200
#define MB_INFO_APM_TABLE                       0x00000400
#define MB_INFO_VBE                             0x00000800
#define MB_INFO_FRAMEBUFFER                     0x00001000

// types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t size_t;


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long      uint64; // cant use this yet, as we are not in 64bit mode
typedef uint32 uintmax_t;

typedef uint8 ui8;
typedef uint16 ui16;
typedef uint32 ui32;

typedef char i8;
typedef short i16;
typedef int i32;

// Register structs for interrupt/exception
typedef struct registers
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
}register_t;

// NULL
#define NULL 0x00


// Using C in c++
#ifdef __cplusplus
#define EXTNC_ extern "C"
#else
#define EXTNC_
#endif

#define EXTNC_START {
#define ECTNC_END }


// Making c++ into a string
#define MacroStrCon(x) #x


// A simple backup Throw
EXTNC_ void ThrowISR(int n);

// va args
typedef __builtin_va_list va_list;
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

// asm stuff
#define Vasm(x) asm volatile(x);
#define NO_INSTRUCTION Vasm("nop");
#define HALT while(1) { NO_INSTRUCTION; } 


#endif // !KERNAL_CORE
