# FluxedOS
![](https://github.com/corigan01/FluxedOS/blob/main/os_pictures/home%20screen.png)


A hobbyist OS project!

## Goals of this project so far
- [x] Interupts
- [x] TEXT MODE VGA 
- [x] C++ Loaded from C
- [ ] ATA (ide) Driver
- [ ] File system
- [ ] Read and Write
- [ ] Install to Drive
- [ ] Boot from Drive
- [ ] Heap / Memory management 
- [ ] Threading
- [ ] Running External Programs 
- [ ] VGA GRAPHICS MODE
- [ ] Basic VGA commands like Draw , DrawLine , DrawBox
- [ ] Really Basic UI for running Programs
- [ ] Add more to the list after all of these are done! :)

# What is this project?
To create an OS in C/C++ that is fast, and is built for modern computers from the ground up

# Commands and stuff inside the os
```bash
# Please note I only update this once and awhile, so commands can change or even be removed completly

you can use help to get this info

-    MEMORY   
'memset'  (adr*, vlu) 
'memread' (adr*)     
-    DISPLAY  
'echo' (...) 
-    SYSTEM   
'exit' (void)
-    DEBUG    
'test' (void)

```

# How to I compile?
All of these commands are in src!
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
&> sudo bash install.sh 

# This line will build and run the OS. 
&> bash autocompile.sh
```
### This is what the build should look like
![](https://github.com/corigan01/FluxedOS/blob/main/os_pictures/build.png)

# How do I contribute?
1. Fork the project
2. Change the code in your project file
3. Submit a pull request with good context on what you changed
4. If the pull request is good, the code is now in the main project!

## File structure
![](https://github.com/corigan01/FluxedOS/blob/main/os_pictures/files.png)





