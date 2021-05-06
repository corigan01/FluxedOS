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
```bash
# This installs the required packages to build [TESTED ON (UBUNTU / LINUX MINT)] 
# NOTE :: You only need to do this step once
# WARNING :: This script uses sudo / apt, please look over any script before you run as sudo
# If you would like to install the packages on your own, or use don't use apt, the packages are
# git 
# g++ 
# gcc
# gcc-multilib 
# g++-multilib -
# qemu-system-x86 
# xorriso 
&> sudo bash build/install.sh 

# This line will build and run the OS. The file is contained outside the src directory. 
&> bash Build.sh
```





## File structure

```bash
.
├── autocompile.sh
├── bin
│   ├── dir.exc
│   └── test.exc
├── boot
│   └── boot.s
├── checksum
│   ├── 09f8e0126764a36489dfec100f1158b9  temp.txt new.txt
│   ├── 2df5ea0438043fb2d22b1035db16ace2  temp.txt new.txt
│   ├── 3d07b651fff69a88e26f1cbc9b5b1cf5  temp.txt new.txt
│   ├── 62110ddc1731722c9ea7cac8f4b044bd  temp.txt new.txt
│   ├── 8a91676008d15f8d6e2f8757e7d91181  temp.txt new.txt
│   ├── d41d8cd98f00b204e9800998ecf8427e  temp.txt new.txt
│   ├── d41d8cd98f00b204e9800998ecf8427e  temp.txt old.txt
│   ├── da7e09838080a5fd70d15a68cf994b39  temp.txt new.txt
│   ├── f113f794444a59656add6d82ba8f5b36  temp.txt new.txt
│   ├──  new.txt
│   └──  old.txt
├── cpu
│   ├── cpu.h
│   ├── gdt.c
│   ├── gdt.h
│   ├── idt.c
│   ├── idt.h
│   ├── irq.c
│   ├── irq.h
│   ├── isr.c
│   └── isr.h
├── grub.cfg
├── install.sh
├── Kernal
│   ├── BUILD.h
│   ├── CppKern.cpp
│   ├── CppKern.h
│   ├── Kernal.c
│   └── Kernal.h
├── lib
│   ├── compare
│   │   ├── compare.cpp
│   │   └── compare.h
│   ├── core
│   │   └── core.h
│   ├── DisplayOut
│   │   ├── displayout.cpp
│   │   └── displayout.h
│   ├── IO
│   │   ├── keyboard.cpp
│   │   ├── Keyboard.h
│   │   ├── port.cpp
│   │   └── port.h
│   ├── String
│   │   ├── String.cpp
│   │   └── String.h
│   ├── Term
│   │   ├── Term.c
│   │   └── Term.h
│   ├── Vector
│   │   ├── vector.cpp
│   │   └── vector.h
│   ├── VGA
│   │   ├── VGA.cpp
│   │   └── VGA.h
│   └── VirtualConsole
│       ├── VirtualConsole.cpp
│       └── VirtualConsole.h
├── linker.ld
├── log
│   ├── A++OUTPUT.txt
│   ├── G++COUTPUT.txt
│   ├── G++OUTPUT.txt
│   ├── GRUB.txt
│   ├── isoLOG.txt
│   └── LINKOUTPUT.txt
└── Proc
    ├── dir
    │   ├── idk.cpp
    │   └── log
    │       └── G++OUTPUT.txt
    └── test
        ├── log
        │   ├── G++OUTPUT.txt
        │   └── LINKOUTPUT.txt
        └── test.cpp

21 directories, 62 files


```
