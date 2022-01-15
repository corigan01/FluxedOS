cmake_minimum_required(VERSION 3.7)

enable_language(ASM_NASM)

project(FluxedOS VERSION 0.0.1 LANGUAGES CXX C ASM_NASM) 

configure_file(src/Kernel/BUILD.h.in src/Kernel/BUILD.h)

MACRO(HEADER_DIRECTORY return_list file)
    FILE(GLOB_RECURSE new_list ${file})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORY(header_dir_list *.h)
HEADER_DIRECTORY(headerpp_dir_list *.hpp)

list(LENGTH header_dir_list header_dir_list_count)
message(STATUS "[INFO] FOUND ${header_dir_list_count} header .h directories!")
list(LENGTH headerpp_dir_list headerpp_dir_list_count)
message(STATUS "[INFO] FOUND ${headerpp_dir_list_count} header .hpp directories!")


set(CMAKE_NASM_LINK_EXECUTABLE "ld   -o  ")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_FLAGS "")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

set ( ASM
    src/boot/boot.asm
    src/CPU/cpu.asm 
    src/CPU/gdt.asm 
    src/CPU/irq.asm 
    src/CPU/isr.asm 
    src/System/panic/stack_dump.asm
)

set ( C 
    src/boot/KenelEntry.c
    src/CPU/cpu.c
    src/CPU/gdt.c
    src/CPU/idt.c
    src/CPU/irq.c
    src/CPU/isr.c
)

set (CXX
    src/lib/BundledContext/BundledContext.cpp
    src/lib/SmartPointer/smartpointer.cpp
    src/lib/StanderdOperations/Operations.cpp
    src/lib/bitmap/bitmap.cpp
    src/lib/string/string.cpp
    src/lib/assert/assert.cpp
    src/lib/vector/k_vector/k_vector.cpp
    src/lib/math/math.cpp
    src/Kernel/kernel.cpp
    src/Kernel/boot.cpp
    src/Kernel/runtime.cpp
    src/System/PCI/pci.cpp
    src/System/Clock/PIT/PIT.cpp
    src/System/Clock/RTC/RTC.cpp
    src/System/CommandHandler/CommandHandler.cpp
    src/System/Console/console.cpp
    src/System/Display/Display.cpp
    src/System/Keyboard/keyboard.cpp
    src/System/Port/port.cpp
    src/System/Power/Power.cpp
    src/System/Serial/serial.cpp
    src/System/cpu/cpu.cpp
    src/System/kout/kernelout/kernelout.cpp
    src/System/kout/kout.cpp
    src/System/memory/vmm.cpp
    src/System/memory/paging/page.cpp
    src/System/memory/pmm/pmm.cpp
    src/System/memory/kmemory.cpp
    src/System/tty/tty.cpp
    src/System/panic/panic.cpp
    src/System/FPU/fpu.cpp
    src/System/Graphics/DisplayServer/ds.cpp
    src/System/Graphics/vbe.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_COMPILE_FEATURES -m32 -g -elf_i386 -O -O2 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c)
set(CMAKE_CXX_COMPILE_FEATURES -m32 -g -I ../src/ -elf_i386 -std=c++2a -O -Wno-write-strings -fstrength-reduce -fno-use-cxa-atexit -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -fdiagnostics-color=always)
add_link_options(-m32 -g -O2 -lstdc++ -nostartfiles -Wno-undef -nostdinc -T ../src/linker.ld )

add_executable(FluxedOS.bin ${ASM} ${C} ${CXX})
target_include_directories(FluxedOS.bin PUBLIC ${header_dir_list}/* ${headerpp_dir_list}/*)

target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,GNU>:
    ${CMAKE_CXX_COMPILE_FEATURES}
>)
target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:C,GNU>:
    ${CMAKE_C_COMPILE_FEATURES}
>)



