# src

This folder contains all the code the os has been built on.

```c
/*
THIS LICENSE APPLIES TO ALL FILES AND FOLDERS IN THIS PROJECT. 
IF YOU CONTRIBUTE, YOUR NAME WILL BE ADDED TO THIS FILE. 

MIT License

Copyright (c) 2021 Main Menu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
```

## Building OS
Please see https://github.com/corigan01/FluxedOS/blob/main/src/build/README.md




## File structure

```bash
├── BuildHeadless.sh
├── Build.sh
├── CODE_OF_CONDUCT.md
├── CONTRIBUTING.md
├── ISO
│   ├── FluxedOS.iso
│   └── README.md
├── LICENSE
├── log
│   ├── GccOUTPUT.txt
│   ├── G++OUTPUT.txt
│   ├── GRUB.txt
│   ├── isoLOG.txt
│   └── LINKOUTPUT.txt
├── os_pictures
│   ├── build.png
│   ├── commands.png
│   ├── files.png
│   ├── grub.png
│   └── home screen.png
├── README.md
├── src
│   ├── bin
│   │   ├── dir.exc
│   │   ├── README.md
│   │   └── test.exc
│   ├── boot
│   │   ├── boot.h
│   │   ├── boot.S
│   │   └── KenelEntry.c
│   ├── build
│   │   ├── autocompile.sh
│   │   ├── func.sh
│   │   ├── install.sh
│   │   └── makeimg.sh
│   ├── Checksum
│   │   ├── sum_new.check
│   │   └── sum_old.check
│   ├── CPU
│   │   ├── cpu.c
│   │   ├── cpu.h
│   │   ├── cpu.S
│   │   ├── gdt.c
│   │   ├── gdt.h
│   │   ├── gdt.S
│   │   ├── idt.c
│   │   ├── idt.h
│   │   ├── irq.c
│   │   ├── irq.h
│   │   ├── irq.S
│   │   ├── isr.c
│   │   ├── isr.h
│   │   └── isr.S
│   ├── disk.img
│   ├── fs
│   │   └── test.txt
│   ├── grub.cfg
│   ├── Kernel
│   │   ├── BUILD.b
│   │   └── kernel.cpp
│   ├── KEYS.txt
│   ├── lib
│   │   ├── bitmap
│   │   │   ├── bitmap.cpp
│   │   │   └── bitmap.hpp
│   │   ├── core
│   │   │   └── core.h
│   │   ├── multiboot
│   │   │   └── multiboot.h
│   │   ├── StanderdOperations
│   │   │   ├── Operations.cpp
│   │   │   └── Operations.hpp
│   │   ├── string
│   │   │   ├── string.cpp
│   │   │   └── string.hpp
│   │   └── vector
│   │       └── vector.hpp
│   ├── linker.ld
│   ├── log
│   │   ├── A++OUTPUT.txt
│   │   ├── diskimg_creation.txt
│   │   ├── GccOUTPUT.txt
│   │   ├── G++COUTPUT.txt
│   │   ├── G++FAILED.txt
│   │   ├── G++OUTPUT.txt
│   │   ├── GRUB.txt
│   │   ├── imgcreate.txt
│   │   ├── isoLOG.txt
│   │   └── LINKOUTPUT.txt
│   ├── README.md
│   ├── System
│   │   ├── cpu
│   │   │   ├── cpu.cpp
│   │   │   └── cpu.hpp
│   │   ├── Display
│   │   │   ├── Display.cpp
│   │   │   └── Display.hpp
│   │   ├── Keyboard
│   │   ├── kout
│   │   │   ├── kout.cpp
│   │   │   └── kout.hpp
│   │   ├── memory
│   │   │   ├── heap.cpp
│   │   │   ├── heap.hpp
│   │   │   ├── paging
│   │   │   │   ├── page.cpp
│   │   │   │   └── page.hpp
│   │   │   └── pmm
│   │   │       ├── pmm.cpp
│   │   │       └── pmm.hpp
│   │   ├── Port
│   │   │   ├── port.cpp
│   │   │   └── port.hpp
│   │   ├── Power
│   │   │   ├── Power.cpp
│   │   │   └── Power.hpp
│   │   ├── Serial
│   │   │   ├── serial.cpp
│   │   │   └── serial.hpp
│   │   └── tty
│   │       ├── tty.cpp
│   │       └── tty.hpp
│   └── test.txt
└── visualize.sh

31 directories, 93 files



```
