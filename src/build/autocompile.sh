#!/bin/bash

source func.sh

cd ..

displayWelcome

#if md5sum_check; then
#    printf "Recompile not needed as nothing has changed! \nRunning build...\n"
#    run_build
#
#    clean
#
#    exit 0
#fi



addToBuild 

declare -i BUILDCOUNT=0


#compile .c , .cpp , and .h file
mkdir obj &> /dev/null

echo "---------------- BUILDING ASM -------------------"
# Compile the asm files
cd boot
for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(s\)$')
do
    BUILDCOUNT=$(( 1 + BUILDCOUNT))
    compilea $OUTPUT
done
mv *.o ../obj
cd ..




#echo "---------------- BUILDING PROC ------------------"
#cd Proc
#for FILES in $(dir)
#do
#cd $FILES
#
#    echo "=== BUILDING $FILES ==="
#
#    for SUB_FILE in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
#    do
#        BUILDCOUNT=$(( 1 + BUILDCOUNT))
#        compileProc $SUB_FILE $FILES &
#    done
#
#    wait
#cd ..
#
#done
#
#cd ..

echo "---------------- BUILDING OS --------------------"

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\|hpp\)$')
do
    
    if [[ $OUTPUT == *"Proc"* ]]; then
        printf "%-40s%-4s\e[0;33mSKIP\e[0;34m\n"  "${OUTPUT:0:40}" " "
    elif [[ $OUTPUT == *".cpp"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilec $OUTPUT &
    elif [[ $OUTPUT == *".c"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilec_ $OUTPUT &
    else
        printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
    fi
done

wait

printf "Working Files : $BUILDCOUNT \n"


echo "---------------- LINKING OS ---------------------"

DisDone "Moving object files"

mv *.o obj/ > /dev/null
rm temp.txt &> /dev/null

ts=$(date +%s%N)
#linking the kernel with kernel.o and boot.o files
if g++ -m32 -lstdc++ -nostartfiles -nostdinc -T linker.ld  obj/*.o -o FluxedOS.bin  &> "log/LINKOUTPUT.txt"; then
    DisDone "Linking FluxedOS.bin"
    PFD=$((($(date +%s%N) - $ts)/1000000))
    printf "%-40s%-4s\e[0;32mDONE - $PFD ms\e[0;34m\n"  "${TEx:0:40}" " "
else

            #ouput the errors
            echo -e "\e[0;31m ------------------ LINK FAILED! ------------------ "
            printf "%s" "$(<log/LINKOUTPUT.txt)"
            echo ""
            echo -e "\e[0;31m ------------------- LINK DONE! ------------------- "
            #rm temp.txt

            clean    


            exit
fi

echo "---------------- BUILDING ISO -------------------"
#check FluxedOS.bin file is x86 multiboot file or not
if grub-file --is-x86-multiboot FluxedOS.bin &> "log/GRUB.txt"; then
    DisDone "Generating grub files"
     
else

            #ouput the errors
            echo -e "\e[0;31m ------------------ GRUB FAILED! ------------------ "
            printf "%s" "$(<log/GRUB.txt)"
            echo ""
            echo -e "\e[0;31m ------------------- GRUB DONE! ------------------- "
  
  
            clean

            exit
fi


#building the iso file
DisDone "Making boot/grub"
mkdir -p isodir/boot/grub &> "log/isoLOG.txt"
DisDone "Making boot/programs"
mkdir -p isodir/programs &> "log/isoLOG.txt"
DisDone "Moving programs"
cp bin/* isodir/programs &> "log/isoLOG.txt"
DisDone "Moving FluxedOS.bin"
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "log/isoLOG.txt"
DisDone "Copying grub.cfg"
cp grub.cfg isodir/boot/grub/grub.cfg &>> "log/isoLOG.txt"
DisDone "Generating ISO"
grub-mkrescue -o FluxedOS.iso isodir &>> "log/isoLOG.txt"


DisDone "Moving ISO to ISO/"

mkdir ../ISO/ &> /dev/null &
cp FluxedOS.iso ../ISO/ &

echo "BUILD IN $((($(date +%s%N) - $tis)/1000000)) ms" 
#run 
echo "---------------- RUNNING BUILD ------------------"

#DisDone "Mount of fs"

echo ""
echo ""
echo "Serial COM1"
echo "-------------------"

run_build

## buffer for output
echo ""
echo "-------------------"

DisDone "Running qemu"



#clean up
clean


exit 0





