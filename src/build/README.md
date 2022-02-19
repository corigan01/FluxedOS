# Build
> The main files to build the OS

## How to install dependencies? 
```bash
# You will need the following packages
# ---------------------------------------
# git
# g++
# gcc-multilib
# g++-multilib
# qemu-system-x86
# xorriso
# nasm
# cmake
# ninja / make

# On deban Systems you can use apt to install everything
&> sudo apt install [package]

# On other systems you will need to install the dependencies manually 
```

## How to clone using the terminal?
```bash
&> git clone https://github.com/corigan01/FluxedOS

&> cd FluxedOS
```

## How to build?
```bash
&> mkdir build
&> cd build
&> cmake -GNinja ..
&> ninja run
...
```



## Issues with the build
```bash
# ERROR
cp: target 'isodir/programs' is not a directory
# Can be solved by re-running the build and using ninja clean
# ----------------------------------------------
# Other errors will be added with their fixes soon
```
