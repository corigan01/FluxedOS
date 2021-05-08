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

# On deban Systems you can use the install.sh script to install everything
&> sudo bash install.sh

# On other systems you will need to install the dependencies manually 
```

## How to clone using the terminal?
```bash
&> git clone https://github.com/corigan01/FluxedOS

&> cd FluxedOS
```

## How to build?
```bash
# This file is inside the main folder FluxedOS/
&> bash Build.sh
```

## What the build output should look like
```bash
# This might be a older version of the build script, but it should look close to what you see. 
```

![image](https://user-images.githubusercontent.com/33582457/117325213-1f457c00-ae56-11eb-9cfb-dcd36231134f.png)

## Issues with the build
```bash
# ERROR
cp: target 'isodir/programs' is not a directory
# Can be solved by re-running the build 
# ----------------------------------------------
# Other errors will be added with their fixes soon
```
