cmake_minimum_required(VERSION 2.9)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(triple i386)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

project(FluxedOS LANGUAGES CXX C ASM_NASM ASM) 

set(CMAKE_NASM_LINK_EXECUTABLE "ld   -o  ")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_FLAGS "")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_FLAGS "-m32 -c")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

add_custom_target(buildver
    COMMAND python3 ../tools/argpass.py
)

set ( ASM
    #src/boot/boot.asm
    src/CPU/cpua.asm 
    src/CPU/gdta.asm 
    src/CPU/irqa.asm 
    src/CPU/isra.asm 
    src/System/panic/stack_dump.asm
)

set ( GAS 
    src/boot/boot.S
    src/System/memory/paging/Paging.S
)

set ( C 
    src/boot/KenelEntry.c
    src/CPU/cpu.c
    src/CPU/gdt.c
    src/CPU/idt.c
    src/CPU/irq.c
    src/CPU/isr.c
    src/lib/StanderdOperations/Operations.c

)

set (CXX
    src/lib/BundledContext/BundledContext.cpp
    src/lib/SmartPointer/smartpointer.cpp
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
    src/System/memory/paging/page.cpp
    src/System/memory/pmm/pmm.cpp
    src/System/memory/kmemory.cpp
    src/System/tty/tty.cpp
    src/System/panic/panic.cpp
    src/System/FPU/fpu.cpp
    src/System/Graphics/DisplayServer/ds.cpp
    src/System/Graphics/vbe.cpp
    src/System/memory/vmm/vmm.cpp
        src/System/CompilerFixes/cxa.cpp
    src/System/CompilerFixes/icxxabi.cpp
    src/System/memory/staticalloc/skmalloc.cpp
    src/System/Disk/ata.cpp
)

set_source_files_properties(${ASM} PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(${GAS} PROPERTIES LANGUAGE ASM)

set(CMAKE_C_COMPILE_FEATURES  -g -m32 -nostdinc  -nostdlib
-fPIE -fomit-frame-pointer -Wall
 -finline-functions -fno-stack-protector
 -fpic -fshort-wchar -mno-red-zone -fno-builtin)
set(CMAKE_CXX_COMPILE_FEATURES -g -m32 -nostdinc  -nostdlib
 -fno-rtti -fPIE -fno-exceptions -Wno-write-strings -Wall
 -fno-stack-protector 
 -fomit-frame-pointer -finline-functions -fno-builtin 
 -fdiagnostics-color=always)
set(CMAKE_ASM_COMPILE_FEATURES -shared )

add_link_options(-m32 -g -Wall -Werror -Wextra -ffreestanding -fno-rtti -fPIE -nodefaultlibs -fno-stack-protector -fno-exceptions -nostartfiles -Wno-undef -nostdlib  -T ../src/linker.ld  )

add_executable(FluxedOS.bin ${GAS} ${ASM} ${C} ${CXX})
add_dependencies(FluxedOS.bin buildver)

target_include_directories(FluxedOS.bin PUBLIC src)

target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:CXX,Clang>:
    ${CMAKE_CXX_COMPILE_FEATURES}
>)
target_compile_options(FluxedOS.bin PUBLIC $<$<COMPILE_LANG_AND_ID:C,Clang>:
    ${CMAKE_C_COMPILE_FEATURES}
>)


add_custom_target(iso
    COMMAND mkdir -p isodir/boot/grub && cp FluxedOS.bin isodir/boot/FluxedOS.bin && cp ../src/grub.cfg isodir/boot/grub/grub.cfg && grub-mkrescue -o FluxedOS.iso isodir && cp FluxedOS.iso ../ISO
    BYPRODUCTS FluxedOS.iso
)
add_dependencies(iso FluxedOS.bin)

add_custom_target(run USES_TERMINAL
    COMMAND  qemu-system-x86_64 -cdrom FluxedOS.iso -drive format=raw,file=disk.img -boot strict=on -cpu max -smp 1,sockets=1,cores=1,threads=1 -display gtk -m 800m -k en-us -serial stdio -nic none
    DEPENDS iso
)

add_custom_target(mkimg USES_TERMINAL
        COMMAND dd if=/dev/zero of=disk.img bs=1M count=50 && mkfs ext2 -F disk.img && mkdir fs &> /dev/null && sudo sudo mount -t ext2 -o loop disk.img fs && sudo sudo chmod 777 -R disk.img fs/
        )

add_custom_target(debug USES_TERMINAL
    COMMAND  objcopy --only-keep-debug FluxedOS.bin FluxedOS.elf && strip --strip-debug --strip-unneeded FluxedOS.bin && qemu-system-x86_64 -s -S FluxedOS.bin -cdrom FluxedOS.iso -boot strict=on -cpu max -smp 1,sockets=1,cores=1,threads=1 -display gtk -m 500m -k en-us -serial stdio &
    DEPENDS iso
)