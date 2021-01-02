#clean up
rm -r isodir
rm FluxedOS.*

#assemble boot.s file
as --32 boot.s -o boot.o
echo "ASM COMPILED"

#compile kernel.c file
for OUTPUT in $(find ./ -type f -iregex ".*\.cpp")
do
echo "$OUTPUT is getting compiled"
screen -d -m g++ -m32 -c $OUTPUT  -ffreestanding -O2 -Wall -Wextra -Wl,-ekernal_entry -fdiagnostics-color=always 
echo "$OUTPUT DONE!"
done

echo "CPP COMPILED"

#linking the kernel with kernel.o and boot.o files
ld -m elf_i386 -T linker.ld Kernal.o String.o boot.o -o FluxedOS.bin -nostdlib
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