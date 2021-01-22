
echo -e "      ________                    __   ______          "
echo "     / ____/ /_  ___  _____  ____/ /  / ____/___  ____ "
echo "    / /_  / / / / / |/_/ _ \/ __  /  / /   / __ \\/ __ \\"
echo "   / __/ / / /_/ />  </  __/ /_/ /  / /___/ /_/ / /_/ /"
echo "  /_/   /_/\__,_/_/|_|\___/\__,_/   \____/ .___/ .___/ "
echo "                                        /_/   /_/      "
echo -e "\e[0;34m-- The easy auto \e[0;31mC++\e[0;34m compiler that runs in your terminal! --"
echo -e "            A \e[0;35mMain Menu\e[0;34m - \e[0;35mQUEST\e[0;34m project!"
echo
echo "---------------- ADDING TO BUILD ----------------"
awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernal/BUILD.h > temp.txt
cp temp.txt Kernal/BUILD.h 
rm temp.txt

clean() {
    echo -e "\e[0;33m---------------- CLEANED UP FILES ---------------"
    rm -r isodir &> /dev/null
    rm FluxedOS.* &> /dev/null
    rm G++OUTPUT.txt &> /dev/null
    rm $(find ./ -type f -iregex '.*/.*\.\(gch\)$') &> /dev/null
    rm *.o &> /dev/null
    
}



printf "%s" "$(<Kernal/BUILD.h)"
echo

#assemble boot.s file
compilea() {
    OUTPUT="$1"

    if as --32 $OUTPUT.s -o boot.o  &> "log/A++OUTPUT.txt"; then
            echo -e "$OUTPUT \t\t\t\t    \e[0;32mDONE\e[0;34m" 
        else

            #delete old checksum files because we need to get the error again
            #for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
            #do
                #echo "$OUTPUT" > temp.txt
                #f_name="checksum/$(md5sum temp.txt)"
                #rm "$f_name old.txt" &> /dev/null
            #done

            #ouput the errors
            echo -e "\e[0;31m ------------------ ASM FAILED! ------------------ "
            printf "%s" "$(<A++OUTPUT.txt)"
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




#compile the given file with g++
compilec() {
    OUTPUT="$1"

    if g++ -m32 -lgcc_s -c $OUTPUT  -ffreestanding -O2 -Wall -Wextra -Wl,-ekernal_entry -fdiagnostics-color=always -lstdc++  &> "log/G++OUTPUT.txt"; then
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
}

echo "---------------- BUILDING CPP -------------------"
#compile .c , .cpp , and .h file
for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
do
    compilec $OUTPUT 
done

wait

rm temp.txt &> /dev/null


echo "---------------- LINKING BUILDS -----------------"

#linking the kernel with kernel.o and boot.o files
if g++ -m32 -T linker.ld *.o -o FluxedOS.bin -nostdlib -lstdc++ -fdiagnostics-color=always -lgcc_s &> "log/LINKOUTPUT.txt"; then
        
        rm *.o &> /dev/null
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

echo "---------------- CREATING GRUB ------------------"
#check FluxedOS.bin file is x86 multiboot file or not
if grub-file --is-x86-multiboot FluxedOS.bin &> "log/GRUB.txt"; then
        
        rm *.o &> /dev/null
else

            #ouput the errors
            echo -e "\e[0;31m ------------------ GRUB FAILED! ------------------ "
            printf "%s" "$(<log/GRUB.txt)"
            echo ""
            echo -e "\e[0;31m ------------------- GRUB DONE! ------------------- "
  
  
            clean

            exit
fi

echo "---------------- BUILDING ISO -------------------"
#building the iso file
mkdir -p isodir/boot/grub &> "log/isoLOG.txt"
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "log/isoLOG.txt"
cp grub.cfg isodir/boot/grub/grub.cfg &>> "log/isoLOG.txt"
grub-mkrescue -o FluxedOS.iso isodir &>> "log/isoLOG.txt"


echo "---------------- COPYING ISO --------------------"
cp FluxedOS.iso ../ISO/

#run
echo "---------------- RUNNING BUILD ------------------"
qemu-system-x86_64 -cdrom FluxedOS.iso -display gtk -vga std


#clean up
clean