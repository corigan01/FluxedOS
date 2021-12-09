#!/bin/bash
source src/build/func.sh

tis=$(date +%s%N)
cd src

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
    compilea $OUTPUT 
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



exit 0 # Build good!
