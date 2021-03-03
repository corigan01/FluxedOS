# Display the welcome
tis=$(date +%s%N)
echo "      ________                    __   ______          "
echo "     / ____/ /_  ___  _____  ____/ /  / ____/___  ____ "
echo "    / /_  / / / / / |/_/ _ \/ __  /  / /   / __ \\/ __ \\"
echo "   / __/ / / /_/ />  </  __/ /_/ /  / /___/ /_/ / /_/ /"
echo "  /_/   /_/\__,_/_/|_|\___/\__,_/   \____/ .___/ .___/ "
echo "                                        /_/   /_/      "
echo -e "\e[0;34m-- The easy auto \e[0;31mC++\e[0;34m compiler that runs in your terminal! --"
echo -e "            A \e[0;35mMain Menu\e[0;34m aka \e[0;35mcorigan01\e[0;34m project!"
echo


addToBuild() {
    # add one to the build number
    echo "---------------- ADDING TO BUILD ----------------"
    awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernal/BUILD.h > temp.txt
    cp temp.txt Kernal/BUILD.h 
    rm temp.txt
    printf "%s" "$(<Kernal/BUILD.h)"
    echo
}

addToBuild &



#clean up files
clean() {
    echo -e "\e[0;33m---------------- CLEANED UP FILES ---------------"
    rm -r isodir &> /dev/null
    rm FluxedOS.* &> /dev/null
    rm G++OUTPUT.txt &> /dev/null
    rm $(find ./ -type f -iregex '.*/.*\.\(gch\)$') &> /dev/null
    rm *.o &> /dev/null
    rm boot/boot.o &> /dev/null
    rm -r obj &> /dev/null
    
}

#assemble boot.s file
compilea() {
    OUTPUT="$1"

    if nasm -f elf $OUTPUT.s -o boot.o  &> "log/A++OUTPUT.txt"; then
            echo -e "$OUTPUT \t\t\t\t    \e[0;32mDONE\e[0;34m" 
        else

            #ouput the errors
            echo -e "\e[0;31m ------------------ ASM FAILED! ------------------ "
            printf "%s" "$(<log/A++OUTPUT.txt)"
            echo ""
            echo -e "\e[0;31m ------------------- ASM DONE! ------------------- "
            #rm temp.txt

            clean

            exit
        fi

}


echo "---------------- BUILDING ASM -------------------"
# Compile the asm files
compilea boot/boot 

compileProc() {
    OUTPUT="$1"
    OUTNAME="$2"

    mkdir log &> /dev/null

    if g++ -m32 -elf_i386 -nostdinc -nostartfiles -lgcc_s $OUTPUT -o "$FILES.exc" -ffreestanding -O2 -Wall -Wextra -fdiagnostics-color=always -lstdc++  &> "log/G++OUTPUT.txt"; then
         printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
    else

        #ouput the errors
        echo -e "\e[0;31m ------------------ CPP FAILED! ------------------ "
        printf "%s" "$(<log/G++OUTPUT.txt)"
        echo ""
        echo -e "\e[0;31m ------------------- CPP DONE! ------------------- "
        #rm temp.txt

        clean

        exit
    fi

    mv "$FILES.exc" ../../bin/
}

#compile the given file with g++
compilec_() {
    OUTPUT="$1"

    mkdir log &> /dev/null
    local ts=$(date +%s%N)

    if gcc -m32 -elf_i386 -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++COUTPUT.txt"; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "%-40s%-4s\e[0;32mDONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " "
    else

        #ouput the errors
        echo -e "\e[0;31m ------------------ C FAILED! -------------------- "
        printf "%s" "$(<log/G++COUTPUT.txt)"
        echo ""
        echo -e "\e[0;31m ------------------- C DONE! --------------------- "
        #rm temp.txt

        clean

        exit
    fi
}

#compile the given file with g++
compilec() {
    OUTPUT="$1"

    mkdir log &> /dev/null
    local ts=$(date +%s%N)

    if g++ -m32 -elf_i386  -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++OUTPUT.txt"; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "%-40s%-4s\e[0;32mDONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " "
    else
        printf "%-40s%-4s\e[0;31mFAILED\e[0;34m\n"  "${OUTPUT:0:40}" " "
        #ouput the errors

        g++ -m32 -elf_i386  -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++OUTPUT.txt"
        printf "%s" "$(<log/G++OUTPUT.txt)"
        echo ""
        #rm temp.txt

        clean

        exit

    fi
}

echo "---------------- BUILDING PROC ------------------"

#compile .c , .cpp , and .h file
mkdir obj &> /dev/null

cd Proc
for FILES in $(dir)
do
cd $FILES

    echo "=== BUILDING $FILES ==="

    for SUB_FILE in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
    do
        compileProc $SUB_FILE $FILES &
    done

    wait
cd ..

done

cd ..

echo "---------------- BUILDING CPP -------------------"

for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
do
    
    if [[ $OUTPUT == *"Proc"* ]]; then
        printf "%-40s%-4s\e[0;33mSKIP\e[0;34m\n"  "${OUTPUT:0:40}" " "
    elif [[ $OUTPUT == *".cpp"* ]]; then
        compilec $OUTPUT &
    elif [[ $OUTPUT == *".c"* ]]; then
        compilec_ $OUTPUT &
    else
        printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
    fi
done

wait


echo "---------------- LINKING BUILDS -----------------"

TEx="Moving object files"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
mv *.o obj/ > /dev/null
rm temp.txt &> /dev/null

ts=$(date +%s%N)
#linking the kernel with kernel.o and boot.o files
if g++ -m32 -lstdc++ -nostartfiles -T linker.ld  obj/*.o -o FluxedOS.bin  &> "log/LINKOUTPUT.txt"; then
    TEx="Linking FluxedOS.bin"
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
    TEx="Generating grub files"
    printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " " 
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
TEx="Making boot/grub"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
mkdir -p isodir/boot/grub &> "log/isoLOG.txt"
TEx="Making boot/programs"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
mkdir -p isodir/programs &> "log/isoLOG.txt"
TEx="Moving programs"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
cp bin/* isodir/programs &> "log/isoLOG.txt"
TEx="Moving FluxedOS.bin"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "log/isoLOG.txt"
TEx="Copying grub.cfg"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
cp grub.cfg isodir/boot/grub/grub.cfg &>> "log/isoLOG.txt"
TEx="Generating ISO"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
grub-mkrescue -o FluxedOS.iso isodir &>> "log/isoLOG.txt"


TEx="Moving ISO to ISO/"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
mkdir ../ISO/ &> /dev/null &
cp FluxedOS.iso ../ISO/ &

echo "BUILD IN $((($(date +%s%N) - $tis)/1000000)) ms" &
#run 
echo "---------------- RUNNING BUILD ------------------"
qemu-img create Diskimg.img 50m &> "log/imgcreate.txt"
TEx="Creating Diskimg.img"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "

qemu-system-x86_64 -cdrom FluxedOS.iso -vga std -display gtk -drive file=Diskimg.img,if=ide,format=raw 
TEx="Running qemu"
rm -r Diskimg.img
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "
TEx="Removing Diskimg.img"
printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${TEx:0:40}" " "






#clean up
clean