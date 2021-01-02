#      ________                    __   ______          
#     / ____/ /_  ___  _____  ____/ /  / ____/___  ____ 
#    / /_  / / / / / |/_/ _ \/ __  /  / /   / __ \/ __ \
#   / __/ / / /_/ />  </  __/ /_/ /  / /___/ /_/ / /_/ /
#  /_/   /_/\__,_/_/|_|\___/\__,_/   \____/ .___/ .___/ 
#                                        /_/   /_/      
# "-- The easy auto C++ compiler scheduler that runs in your terminal! --"
# "by Main Menu - QUEST"
#


#clean up
rm -r isodir &> /dev/null
rm FluxedOS.* &> /dev/null


#assemble boot.s file
as --32 boot.s -o boot.o
echo "ASM COMPILED"

compile() {
    OUTPUT="$1"
    #echo "$OUTPUT" > temp.txt
    #f_name="checksum/$(md5sum temp.txt)"
    #md5sum  $OUTPUT > "$f_name new.txt"

    #if diff "$f_name new.txt" "$f_name old.txt" &> /dev/null; then
    #    echo "$OUTPUT NOT CHANGED!"
    #else
        #md5sum $OUTPUT > "$f_name old.txt"

        if g++ -m32 -c $OUTPUT  -ffreestanding -O2 -Wall -Wextra -Wl,-ekernal_entry -fdiagnostics-color=always &> "G++OUTPUT.txt"; then
            echo "$OUTPUT Compiled!"
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
            echo " ------------------- G++ DONE! ------------------- "
            #rm temp.txt
            exit
        fi
    #fi
}

#compile .cpp and .h file
for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
do
compile $OUTPUT
done
wait

rm temp.txt &> /dev/null

echo "CPP COMPILED"

#linking the kernel with kernel.o and boot.o files
if ld -m elf_i386 -T linker.ld Kernal.o String.o boot.o -o FluxedOS.bin -nostdlib &> "G++OUTPUT.txt"; then
            echo "$OUTPUT Compiled!"
        else

            #delete old checksum files because we need to get the error again
            #for OUTPUT in $(find ./ -type f -iregex '.*/.*\.\(c\|cpp\|h\)$')
            #do
                #echo "$OUTPUT" > temp.txt
                #f_name="checksum/$(md5sum temp.txt)"
                #rm "$f_name old.txt" &> /dev/null
            #done

            #ouput the errors
            echo " ------------------ LINK FAILED! ------------------ "
            printf "%s" "$(<G++OUTPUT.txt)"
            echo ""
            echo " ------------------- LINK DONE! ------------------- "
            #rm temp.txt
            exit
        fi
echo "CPP + ASM LINKED"

#check FluxedOS.bin file is x86 multiboot file or not
grub-file --is-x86-multiboot FluxedOS.bin
echo "GRUB FILE MADE"

#building the iso file
mkdir -p isodir/boot/grub
cp FluxedOS.bin isodir/boot/FluxedOS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o FluxedOS.iso isodir
echo "ISO FILE MADE"


#run it in qemu
screen -d -m qemu-system-x86_64 -cdrom FluxedOS.iso
echo "QEMU RAN"