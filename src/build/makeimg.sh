dd if=/dev/zero of=disk.img bs=1M count=50
mkfs ext2 -F disk.img
mkdir fs &> /dev/null
sudo sudo mount -t ext2 -o loop disk.img fs
sudo sudo chmod 777 -R disk.img fs/
