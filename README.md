# FluxedOS
### VM
![image](https://user-images.githubusercontent.com/33582457/154788745-8efef18f-b7c3-4f31-af9c-09347888ad65.png)

### Real
![image](https://user-images.githubusercontent.com/33582457/117327467-3e450d80-ae58-11eb-910c-dcb276328cd3.png)


A hobbyist OS project!

## Goals of this project so far
- [x] Interupts
- [x] TEXT MODE VGA 
- [x] C++ Loaded from C
- [x] IRQ Handler
- [x] PIT
- [x] PIC
- [ ] ATA (ide) GXDriver
- [ ] File system
- [ ] Read and Write
- [ ] Install to Drive
- [ ] Boot from Drive
- [X] Heap / Memory management 
- [ ] Threading
- [ ] Running External Programs 
- [X] VGA GRAPHICS MODE
- [X] Basic VGA commands like Draw , DrawLine , DrawBox
- [X] Really Basic UI for running Programs
- [X] Higher Half Kernel
- [X] Paging Support
- [X] Windowing System
- [ ] UserSpace (SOON.tm)
- [ ] Add more to the list after all of these are done! :)

# What is this project?
To create an OS in C/C++ that is fast, and is built for modern computers from the ground up



# How to compile?
All of these commands are in src!
See [HERE](https://github.com/corigan01/FluxedOS/blob/main/src/build/README.md) for build instructions. 

### Needed Packages:
* Cmake
* Ninja
* clang
* qemu
* nasm
* gcc / g++
* grub

### Basic Build Instructions
```bash
git clone https://github.com/corigan01/FluxedOS
cd FluxedOS

mkdir build
cd build

cmake -GNinja .. # If you want to use make then use "cmake .." instead
ninja run
```


# How do I contribute?
1. Fork the project
2. Change the code in your project file
3. Submit a pull request with good context on what you changed
4. If the pull request is good, the code is now in the main project!

If you need more help use this https://github.com/corigan01/FluxedOS/blob/main/CONTRIBUTING.md. 

# Non-code contributing
* If you want to speak about the topics and issues with the OS, please do so in the Issues page. 
* Issues / Spelling Mistakes you find can be reported in issues. 
* If you would like to email me directly about a topic or issue; do so here (corigan01@gmail.com)  

# Support my work
* I don't ask for donations yet, I only ask for a star or a fork on my project!
* If you want to do something else, email me at (corigan01@gmail.com) and we can work something out. 

## Please note if you do want to email me
* #### You don't have to write a formal email 
* #### I don't care what spicy words you use 
* #### I should be able to respond to every email in just a few minutes
* #### You can send basicly anything normal (*no viruses, no p\*rn, no garbage*)


