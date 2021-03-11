#!/bin/bash


addToBuild() {
    # add one to the build number
    echo "---------------- ADDING TO BUILD ----------------"
    awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernel/BUILD.h > temp.txt
    cp temp.txt Kernel/BUILD.h 
    rm temp.txt
    printf "%s" "$(<Kernel/BUILD.h)"
    echo
}

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

    if nasm -f elf $OUTPUT -o $OUTPUT.o  &> "../log/A++OUTPUT.txt"; then
            printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
        else

            #ouput the errors
            echo -e "\e[0;31m ------------------ ASM FAILED! ------------------ "
            printf "%s" "$(<../log/A++OUTPUT.txt)"
            echo ""
            echo -e "\e[0;31m ------------------- ASM DONE! ------------------- "
            #rm temp.txt

            clean

            exit
        fi

}

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


    if gcc -m32 -elf_i386  -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/GccOUTPUT.txt"; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "%-40s%-4s\e[0;32mDONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " "
    else
        printf "%-40s%-4s\e[0;31mFAILED\e[0;34m\n"  "${OUTPUT:0:40}" " "
        #ouput the errors

        gcc -m32 -elf_i386 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/GccOUTPUT.txt"
        printf "%s" "$(<log/GccOUTPUT.txt)"
        echo ""
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


DisDone() {
    OUTPUT="$1"

    printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
}

