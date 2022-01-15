cmake_minimum_required(VERSION 3.1)

project(FluxedOS VERSION 0.0.1)

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

add_compile_options( -m32 -g -I ../src/ -elf_i386 -O -O2 -fno-use-cxa-atexit -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin)
add_link_options(-m32 -g -O2 -lstdc++ -nostartfiles -Wno-undef -nostdinc)


add_executable(FluxedOS.bin src/Kernel/boot.cpp src/CPU/cpu.c src/CPU/gdt.c src/CPU/idt.c src/CPU/irq.c src/CPU/isr.c src/Kernel/kernel.cpp src/Kernel/boot.cpp src/Kernel/runtime.cpp src/System/Clock/PIT/PIT.cpp src/System/Clock/RTC/RTC.cpp src/System/CommandHandler/CommandHandler.cpp src/System/Console/console.cpp src/System/Display/Display.cpp src/System/Keyboard/keyboard.cpp src/System/PCI/pci.cpp src/System/Port/port.cpp src/System/Power/Power.cpp src/System/Serial/serial.cpp src/System/cpu/cpu.cpp src/System/kout/kout.cpp src/System/kout/kernelout/kernelout.cpp src/System/memory/paging/page.cpp src/System/memory/pmm/pmm.cpp src/System/memory/vmm.cpp src/System/memory/kmemory.cpp src/System/panic/panic.cpp src/System/tty/tty.cpp src/System/Graphics/DisplayServer/ds.cpp src/System/Graphics/vbe.cpp src/System/FPU/fpu.cpp src/boot/KenelEntry.c src/lib/BundledContext/BundledContext.cpp src/lib/SmartPointer/smartpointer.cpp src/lib/StanderdOperations/Operations.cpp src/lib/assert/assert.cpp src/lib/bitmap/bitmap.cpp src/lib/string/string.cpp src/lib/vector/k_vector/k_vector.cpp src/lib/math/math.cpp)
target_include_directories(FluxedOS.bin PUBLIC ${header_dir_list} ${headerpp_dir_list})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS "-m32 -g -elf_i386 -O -O2 -fno-use-cxa-atexit -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c")