cmake_minimum_required(VERSION 3.7)

enable_language(ASM_NASM)

project(FluxedOS VERSION 0.0.1 LANGUAGES CXX C ASM_NASM ASM) 

set(CMAKE_NASM_LINK_EXECUTABLE "ld   -o  ")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_FLAGS "")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_FLAGS "-m32")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")


add_custom_target(buildver
    COMMAND python3 ../tools/argpass.py
)

set ( ASM
    #src/boot/boot.asm
    src/CPU/cpu.asm 
    src/CPU/gdt.asm 
    src/CPU/irq.asm 
    src/CPU/isr.asm 
    src/System/panic/stack_dump.asm
)

set ( GAS 
    src/boot/boot.S
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
    src/System/memory/vmm/vmm.cpp
    src/boot/cppentry.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_source_files_properties(${ASM} PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(${GAS} PROPERTIES LANGUAGE ASM)

set(CMAKE_C_COMPILE_FEATURES -m32 -g -elf_i386 -O -O2 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c)
set(CMAKE_CXX_COMPILE_FEATURES -m32 -g -elf_i386 -std=c++2a -O -Wno-write-strings -fstrength-reduce -fno-use-cxa-atexit -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -fdiagnostics-color=always)
add_link_options(-m32 -g -O2 -lstdc++ -nostartfiles -Wno-undef -nostdinc -T ../src/linker.ld )

add_executable(FluxedOS.bin ${GAS} ${ASM} ${C} ${CXX})
add_dependencies(FluxedOS.bin buildver)

target_include_directories(FluxedOS.bin PUBLIC src)

target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,GNU>:
    ${CMAKE_CXX_COMPILE_FEATURES}
>)
target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:C,GNU>:
    ${CMAKE_C_COMPILE_FEATURES}
>)


add_custom_target(FluxedOS.iso
    COMMAND mkdir -p isodir/boot/grub && cp FluxedOS.bin isodir/boot/FluxedOS.bin && cp ../src/grub.cfg isodir/boot/grub/grub.cfg && grub-mkrescue -o FluxedOS.iso isodir && cp FluxedOS.iso ../ISO
    DEPENDS FluxedOS.bin
)

add_custom_target(run USES_TERMINAL
    COMMAND  qemu-system-x86_64 -cdrom FluxedOS.iso -boot strict=on -cpu max -smp 1,sockets=1,cores=1,threads=1 -display gtk -m 500m -k en-us -serial stdio &
    DEPENDS FluxedOS.iso
)