#ifndef KERNAL_CORE
#define KERNAL_CORE


// types
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t size_t;


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

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


// asm stuff
#define Vasm(x) asm volatile(x);
#define NO_INSTRUCTION Vasm("nop");
#define HALT while(1) { NO_INSTRUCTION; } 



// PLEASE FIX THIS!!!! USE PIC
#define sleep(x) for (int __dj = 0; __dj > x; __dj++) {;};

#endif // !KERNAL_CORE
