echo "Please make sure you look over this file before you run 'sudo' on it!"
sleep 2

echo "UPDATING YOUR SYSTEM"
sudo pacman -Syyu

echo "INSTALLING PACKAGES TO BUILD FluxedOS"
sudo pacman -S nasm lib32-glibc lib32-gcc-libs 
sudo pacman -S grub xorriso 
sudo pacman -S git g++ gcc 

echo "Done! Now run ./autocompile.sh or bash autocompile.sh to build FluxedOS"