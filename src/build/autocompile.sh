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

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(s\)$')
do
    BUILDCOUNT=$(( 1 + BUILDCOUNT))
    compilea $OUTPUT &
done

wait
cd ..


echo "$((($(date +%s%N) - $tis)/1000000)) ms" 
echo "---------------- BUILDING OS --------------------"

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\|hpp\)$')
do
    compilestuff $OUTPUT &
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

Link_and_check & # Link the bin and check everything

#building the iso file
DisDone "Making boot/grub" 
mkdir -p isodir/boot/grub & &> "log/isoLOG.txt"
DisDone "Making boot/programs"
mkdir -p isodir/programs & &> "log/isoLOG.txt"
DisDone "Moving programs"
cp bin/* isodir/programs & &> "log/isoLOG.txt"

wait

DisDone "Moving FluxedOS.bin"
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "log/isoLOG.txt"
DisDone "Copying grub.cfg"
cp grub.cfg isodir/boot/grub/grub.cfg &>> "log/isoLOG.txt"
DisDone "Generating ISO"
grub-mkrescue -o FluxedOS.iso isodir &>> "log/isoLOG.txt"


DisDone "Moving ISO to ISO/"

mkdir ../ISO/ &> /dev/null &
cp FluxedOS.iso ../ISO/ &

echo "Final Build time in $((($(date +%s%N) - $tis)/1000000)) ms" 
#run 
echo "---------------- RUNNING BUILD ------------------"

#DisDone "Mount of fs"


echo ""
echo ""
echo "Serial COM1"
echo "-------------------"

run_build

## buffer for output
#echo ""
echo "-------------------"

DisDone "Running qemu"

#clean up
clean

exit 0