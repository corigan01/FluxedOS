#!/bin/bash

displayWelcome() {
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
}

md5sum_check() {
    mkdir Checksum &> /dev/null
    md5sum $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\|s\|S\|hpp\)$' ) &> Checksum/sum_new.check

    if cmp  $"Checksum/sum_new.check" $"Checksum/sum_old.check"; then
        cp Checksum/sum_new.check Checksum/sum_old.check 
        return 0
    else
        cp Checksum/sum_new.check Checksum/sum_old.check
        return 1
    fi
}

addToBuild() {
    # add one to the build number
    echo "---------------- ADDING TO BUILD ----------------"
    awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernel/BUILD.b > temp.txt
    cp temp.txt Kernel/BUILD.b 
    rm temp.txt
    printf "%s" "$(<Kernel/BUILD.b)"
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

            exit -1
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

        exit -1
    fi

    mv "$FILES.exc" ../../bin/
}

#compile the given file with g++
compilec_() {
    OUTPUT="$1"

    mkdir log &> /dev/null
    local ts=$(date +%s%N)


    if gcc -m32 -elf_i386  -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/GccOUTPUT.txt"; then
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

        exit -1

    fi
    
}

#compile the given file with g++
compilec() {
    OUTPUT="$1"

    mkdir log &> /dev/null
    local ts=$(date +%s%N)

    if g++ -m32 -elf_i386 -std=c++2a -O -fstrength-reduce -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++OUTPUT.txt"; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "%-40s%-4s\e[0;32mDONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " "
    else
        printf "%-40s%-4s\e[0;31mFAILED\e[0;34m\n"  "${OUTPUT:0:40}" " "
        #ouput the errors

        g++ -m32 -elf_i386 -std=c++2a -O -fstrength-reduce -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++OUTPUT.txt"
        printf "%s" "$(<log/G++OUTPUT.txt)"
        echo ""
        #rm temp.txt

        clean

        exit -1

    fi
}


DisDone() {
    OUTPUT="$1"

    printf "%-40s%-4s\e[0;32mDONE\e[0;34m\n"  "${OUTPUT:0:40}" " "
}

run_build() {
    
    qemu-system-x86_64                          \
        -cdrom ../ISO/FluxedOS.iso              \
        -vga std                                \
        -boot strict=on                         \
        -cpu max                                \
        -smp 1,sockets=1,cores=1,threads=1      \
        -display gtk                            \
        -drive file=disk.img,if=ide,format=raw  \
        -m 500m                                 \
        -k en-us                                \
        -serial stdio
}