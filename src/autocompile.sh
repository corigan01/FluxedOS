echo "FluxedCpp Started!"
#      ________                    __   ______          
#     / ____/ /_  ___  _____  ____/ /  / ____/___  ____ 
#    / /_  / / / / / |/_/ _ \/ __  /  / /   / __ \/ __ \
#   / __/ / / /_/ />  </  __/ /_/ /  / /___/ /_/ / /_/ /
#  /_/   /_/\__,_/_/|_|\___/\__,_/   \____/ .___/ .___/ 
#                                        /_/   /_/      
# "-- The easy auto C++ compiler scheduler that runs in your terminal! --"
# "by Main Menu - QUEST"
#

awk '{if ($1 == $1) print $1, $2, $3 + 1;}' Kernal/BUILD.h > temp.txt
cp temp.txt Kernal/BUILD.h 
rm temp.txt

clean() {
    rm -r isodir &> /dev/null
    rm FluxedOS.* &> /dev/null
    rm G++OUTPUT.txt &> /dev/null
    rm $(find ./ -type f -iregex '.*/.*\.\(gch\)$') &> /dev/null
    rm *.o &> /dev/null
    echo "CLEANED FILES"
}

rm *.o &> /dev/null
echo "CLEANED LIB FILES"


printf "%s" "$(<Kernal/BUILD.h)"
echo

#assemble boot.s file
compilea() {
    OUTPUT="$1"

    if fasm  $OUTPUT.s $.o  &> "A++OUTPUT.txt"; then
            echo -e "$OUTPUT  DONE" 
        else

            #delete old checksum files because we need to get the error again
            #for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
            #do
                #echo "$OUTPUT" > temp.txt
                #f_name="checksum/$(md5sum temp.txt)"
                #rm "$f_name old.txt" &> /dev/null
            #done

            #ouput the errors
            echo " ------------------ ASM FAILED! ------------------ "
            printf "%s" "$(<A++OUTPUT.txt)"
            echo ""
            echo " ------------------- ASM DONE! ------------------- "
            #rm temp.txt

            clean

            exit
        fi

    
    
}

compilea boot/boot
compilea boot/loader

echo "ASM COMPILED"


compilec() {
    OUTPUT="$1"
    #echo "$OUTPUT" > temp.txt
    #f_name="checksum/$(md5sum temp.txt)"
    #md5sum  $OUTPUT > "$f_name new.txt"

    #if diff "$f_name new.txt" "$f_name old.txt" &> /dev/null; then
    #    echo "$OUTPUT NOT CHANGED!"
    #else
        #md5sum $OUTPUT > "$f_name old.txt"

        if g++ -m32 -lgcc_s -c $OUTPUT  -ffreestanding -O2 -Wall -Wextra -Wl,-ekernal_entry -fdiagnostics-color=always -lstdc++  &> "G++OUTPUT.txt"; then
            echo -e "$OUTPUT  DONE" 
        else

            #delete old checksum files because we need to get the error again
            #for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
            #do
                #echo "$OUTPUT" > temp.txt
                #f_name="checksum/$(md5sum temp.txt)"
                #rm "$f_name old.txt" &> /dev/null
            #done
            

            #ouput the errors
            echo " ------------------ CPP FAILED! ------------------ "
            printf "%s" "$(<G++OUTPUT.txt)"
            echo ""
            echo " ------------------- CPP DONE! ------------------- "
            #rm temp.txt

            clean

            exit
        fi
    #fi
}

#compile .cpp and .h file
for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
do
    
    compilec $OUTPUT 

done

wait

rm temp.txt &> /dev/null

echo "CPP COMPILED"

#linking the kernel with kernel.o and boot.o files
if g++ -m32 -T linker.ld *.o -o FluxedOS.bin -nostdlib -lstdc++ -lgcc_s &> "LINKOUTPUT.txt"; then
        echo "CPP + ASM LINKED"
        rm *.o &> /dev/null
else

            #ouput the errors
            echo " ------------------ LINK FAILED! ------------------ "
            printf "%s" "$(<LINKOUTPUT.txt)"
            echo ""
            echo " ------------------- LINK DONE! ------------------- "
            #rm temp.txt

            clean    


            exit
fi


#check FluxedOS.bin file is x86 multiboot file or not

if grub-file --is-x86-multiboot FluxedOS.bin &> "GRUB.txt"; then
        echo "GRUB FILE MADE"
        rm *.o &> /dev/null
else

            #ouput the errors
            echo " ------------------ GRUB FAILED! ------------------ "
            printf "%s" "$(<GRUB.txt)"
            echo ""
            echo " ------------------- GRUB DONE! ------------------- "
  
  
            clean

            exit
fi

#building the iso file
mkdir -p isodir/boot/grub &> "isoLOG.txt"
cp FluxedOS.bin isodir/boot/FluxedOS.bin &>> "isoLOG.txt"
cp grub.cfg isodir/boot/grub/grub.cfg &>> "isoLOG.txt"
grub-mkrescue -o FluxedOS.iso isodir &>> "isoLOG.txt"
echo "ISO FILE MADE"

cp FluxedOS.iso ../ISO/

#run it in qemu
#screen -d -m 
qemu-system-x86_64 -cdrom FluxedOS.iso -display gtk -vga std
echo "QEMU RAN"

#clean up
clean