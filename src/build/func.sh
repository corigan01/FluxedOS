#!/bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

#
# Display the welcome
#
displayWelcome() {
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

#
# Check the files for changes
#
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

#
# Add 1 to the Build number defined in Build.b
#
addToBuild() {
    # add one to the build number
    echo "---------------- ADDING TO BUILD ----------------"
    awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernel/BUILD.b > temp.txt
    cp temp.txt Kernel/BUILD.b 
    rm temp.txt
    printf "%s" "$(<Kernel/BUILD.b)"
    echo
}

#
# Remove all the working files
#
clean() {
    echo -e "\e[0;33m---------------- CLEANED UP FILES ---------------"
    rm -r isodir &> /dev/null
    rm FluxedOS.* &> /dev/null
    rm G++OUTPUT.txt &> /dev/null
    rm $(find ./ -type f -iregex '.*/.*\.\(gch\)$') &> /dev/null
    rm $(find ./ -type f -iregex '.*/.*\.\(o\)$') &> /dev/null
    rm *.o &> /dev/null
    rm boot/boot.o &> /dev/null
    rm -r obj &> /dev/null
    
}

#
# assemble XX.s files [ .S = nasm ]
#
compilea() {
    OUTPUT="$1"
    touch "log/A++OUTPUT.txt" &> /dev/null
    local ts=$(date +%s%N)

    if nasm -f elf $OUTPUT -o $OUTPUT.o  &> "log/A++OUTPUT.txt"; then
        local PFD=$((($(date +%s%N) - $ts)/1000000))
        printf "%-40s%-4s\e[0;32m  $bold DONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    else

        #ouput the errors
        echo -e "\e[0;31m ------------------ ASM FAILED! ------------------ "
        printf "%s" "$(<log/A++OUTPUT.txt)"
        echo ""
        echo -e "\e[0;31m ------------------- ASM DONE! ------------------- "
        #rm temp.txt

        clean

        exit -1
    fi

}

#
# assemble XX.asm files [ .asm = as ]
#
compilegas() {
    OUTPUT="$1"
    touch "log/A++OUTPUT.txt" &> /dev/null
    local ts=$(date +%s%N)

    if as --32 $OUTPUT -o $OUTPUT.o  &> "log/A++OUTPUT.txt"; then
        local PFD=$((($(date +%s%N) - $ts)/1000000))
        printf "%-40s%-4s\e[0;32m  $bold DONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    else

        #ouput the errors
        echo -e "\e[0;31m ------------------ GAS FAILED! ------------------ "
        printf "%s" "$(<log/A++OUTPUT.txt)"
        echo ""
        echo -e "\e[0;31m ------------------- GAS DONE! ------------------- "
        #rm temp.txt

        clean

        exit -1
    fi

}



#
# Asm Helper for detecting the type of asm file
#
compileasm() {
    OUTPUT="$1"
    
    if [[ $OUTPUT == *".S"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilea $OUTPUT 
    elif [[ $OUTPUT == *".asm"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilegas $OUTPUT 
    else
        printf "%-40s%-4s\e[0;32m  $bold DONE\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    fi


}

compileProc() {
    OUTPUT="$1"
    OUTNAME="$2"

    mkdir log &> /dev/null
    la
    if g++ -m32 -elf_i386 -nostdinc -nostartfiles -fno-use-cxa-atexit -lgcc_s $OUTPUT -o "$FILES.exc" -ffreestanding -O2 -Wall -Wextra -fdiagnostics-color=always -lstdc++ -Map KernelSym.txt  &> "log/G++OUTPUT.txt"; then
         printf "%-40s%-4s\e[0;32m  $bold DONE\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
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


    if cc -m32 -g -I src/ -elf_i386  -O -O2 -fno-use-cxa-atexit -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c  $OUTPUT -fdiagnostics-color=always &> /dev/null; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "[ C ] %-40s%-4s\e[0;32m  $bold DONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    else
        printf "[ C ] %-40s%-4s\e[0;31mFAILED\e[0;34m\n"  "${OUTPUT:0:40}" " "
        #ouput the errors

        cc -m32 -I src/ -elf_i386  -O -O2 -fno-use-cxa-atexit -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -c  $OUTPUT -fdiagnostics-color=always &> "log/GccOUTPUT.txt"
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

    if c++ -m32 -g -I src/ -elf_i386 -std=c++2a -O -fstrength-reduce -fno-use-cxa-atexit -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always &> /dev/null; then
         local PFD=$((($(date +%s%N) - $ts)/1000000))
         printf "[C++] %-40s%-4s\e[0;32m  $bold DONE - $PFD ms\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    else
        printf "%-40s%-4s\e[0;31mFAILED\e[0;34m\n"  "${OUTPUT:0:40}" " "
        #ouput the errors

        c++ -m32 -I src/ -elf_i386 -std=c++2a -O -fstrength-reduce -fno-use-cxa-atexit -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -c  $OUTPUT -fdiagnostics-color=always  &> "log/G++OUTPUT.txt"
        printf "[C++] %s" "$(<log/G++OUTPUT.txt)"
        echo ""
        #rm temp.txt

        clean

        exit -1

    fi
}

compilestuff() {
    OUTPUT="$1"
    if [[ $OUTPUT == *"Proc"* ]]; then
        printf "[PRG] %-40s%-4s\e[0;33mSKIP\e[0;34m\n"  "${OUTPUT:0:40}" " "
    elif [[ $OUTPUT == *".cpp"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilec $OUTPUT 
    elif [[ $OUTPUT == *".c"* ]]; then
        BUILDCOUNT=$(( 1 + BUILDCOUNT))
        compilec_ $OUTPUT 
    else
        printf "[ H ] %-40s%-4s\e[0;32m  $bold DONE\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
    fi
}


Link_and_check() {
    ts=$(date +%s%N)
    #linking the kernel with kernel.o and boot.o files
    if g++ -m32 -g -O2 -lstdc++ -nostartfiles -Wno-undef -nostdinc -T linker.ld  obj/*.o -o FluxedOS.bin   &> "log/LINKOUTPUT.txt"; then
        TEx="Linking FluxedOS.bin"
        PFD=$((($(date +%s%N) - $ts)/1000000))
        printf "%-40s%-4s\e[0;32m  $bold DONE - $PFD ms\e[0;34m\n"  "${TEx:0:40}" " $normal"

        DisDone "Stripping Symbols"
        objcopy --only-keep-debug FluxedOS.bin FluxedOS.debug
        strip --strip-debug --strip-unneeded FluxedOS.bin
        
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
    echo "$((($(date +%s%N) - $tis)/1000000)) ms" 
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
}

DisDone() {
    OUTPUT="$1"

    printf "%-40s%-4s\e[0;32m  $bold DONE\e[0;34m\n"  "${OUTPUT:0:40}" " $normal"
}

run_build() {
    
    qemu-system-x86_64                          \
        -cdrom ../ISO/FluxedOS.iso              \
        -boot strict=on                         \
        -cpu max                                \
        -smp 1,sockets=1,cores=1,threads=1      \
        -display gtk                            \
        -drive file=disk.img,if=ide,format=raw  \
        -m 500m                                 \
        -k en-us                                \
        -serial stdio                           \
        -vga std
        #-vga [std|cirrus|vmware|qxl|xenfb|tcx|cg3|virtio|none]

}

run_build_backup() {
    
    qemu-system-x86_64                          \
        -kernel isodir/boot/FluxedOS.bin        \
        -boot strict=on                         \
        -cpu max                                \
        -smp 1,sockets=1,cores=1,threads=1      \
        -display gtk                            \
        -drive file=disk.img,if=ide,format=raw  \
        -m 500m                                 \
        -k en-us                                \
        -serial stdio                           \
        -vga std
        #-vga [std|cirrus|vmware|qxl|xenfb|tcx|cg3|virtio|none]

}