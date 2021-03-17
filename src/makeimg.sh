dd if=/dev/zero of=disk.img bs=1M count=50 &> "log/imgcreate.txt"
DisDone "Creating disk.img"
#cfdisk Diskimg.img
mkfs ext2 -F disk.img 
DisDone "MkFS of disk.img"
sudo mount -t ext2 -o loop disk.img fs

sudo chmod 777 -R *