echo "UPDATING YOUR SYSTEM"
sudo apt update
echo "UPGRADING YOUR SYSTEM"
sudo apt upgrade -y

echo "INSTALLING PACKAGES TO BUILD FluxedOS"
sudo apt install git -y
sudo apt install g++ -y
sudo apt install gcc-multilib -y
sudo apt install g++-multilib -y 
sudo apt install qemu-system-x86 -y
sudo apt install xorriso -y

echo "Done! Now run ./autocompile.sh or bash autocompile.sh to build FluxedOS"