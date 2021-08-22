#!/bin/bash
#!/usr/bin/env bash

tis=$(date +%s%N)
source func.sh
cd ..

displayWelcome 

addToBuild 

declare -i BUILDCOUNT=0

#compile .c , .cpp , and .h file
mkdir obj &> /dev/null

echo "$((($(date +%s%N) - $tis)/1000000)) ms" 
echo "---------------- BUILDING ASM -------------------"
# Compile the asm files

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(s\|asm\)$')
do
    BUILDCOUNT=$(( 1 + BUILDCOUNT))
    compileasm $OUTPUT &
done

wait
cd ..


echo "$((($(date +%s%N) - $tis)/1000000)) ms" 
echo "---------------- BUILDING OS --------------------"

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\|hpp\)$')
do
    compilestuff $OUTPUT &
    BUILDCOUNT=$(( 1 + BUILDCOUNT))
done

wait


printf "Working Files : $BUILDCOUNT \n"

echo "$((($(date +%s%N) - $tis)/1000000)) ms" 
echo "---------------- LINKING OS ---------------------"

DisDone "Moving object files"

mkdir src/obj &> /dev/null
mv $(find ./ -type f -iregex '.*/.*\.\(o\)$') src/obj
rm temp.txt &> /dev/null

cd src

Link_and_check  # Link the bin and check everything

#building the iso file
DisDone "Making boot/grub" 
mkdir -p isodir/boot/grub  &> "log/isoLOG.txt"
#DisDone "Making boot/programs"
#mkdir -p isodir/programs  &> "log/isoLOG.txt"
#DisDone "Moving programs"
#cp bin/* isodir/programs  &> "log/isoLOG.txt"


DisDone "Moving FluxedOS.bin"
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "log/isoLOG.txt"
DisDone "Copying grub.cfg"
cp grub.cfg isodir/boot/grub/grub.cfg  &>> "log/isoLOG.txt"

# Yay its fixed
grub-mkrescue -o FluxedOS.iso isodir &>> "log/isoLOG.txt"


mkdir ../ISO/ &> /dev/null &

if cp FluxedOS.iso ../ISO/ &> /dev/null ; then
    DisDone "Generating ISO"
    DisDone "ISO created"
else
    echo -e "\e[0;31m--------------- ISO BUILD FAILED! --------------- "
    printf "%s" "$(<log/isoLOG.txt)"
    echo ""
    echo -e "\e[0;31m------------------------------------------------- \n\n"

    echo -e "\e[0;31m ATTEMPTING TO LAUNCH WITH BUILD-IN MULTIBOOT! "
    echo -e "\e[0;31m      THIS CAN CAUSE ISSUES WITH THE OS! "
    echo -e "\e[0;31m------------------------------------------------- "


    echo ""
    echo "                  Serial COM1                       "
    echo -e "-------------------------------------------------\e[0;36m "

    run_build_backup 

    ## buffer for output
    #echo ""
    echo -e "\e[0;34m-------------------------------------------------"
    echo ""

    clean
    exit
fi

echo "Final Build time in $((($(date +%s%N) - $tis)/1000000)) ms" 
#run 
echo "---------------- RUNNING BUILD ------------------"

#DisDone "Mount of fs"


echo ""
echo "                  Serial COM1                       "
echo -e "-------------------------------------------------\e[0;36m "

run_build

## buffer for output
#echo ""
echo -e "\e[0;34m-------------------------------------------------"
echo ""


DisDone "Running qemu"

#clean up
clean

cd build
