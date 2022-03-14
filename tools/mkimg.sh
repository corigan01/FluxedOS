#!/bin/bash

cd ../build

echo -e "\e[0;34mUnmounting disk.img"
sudo umount disk.img &> /dev/null
sudo umount /dev/loop0 &> /dev/null
sudo losetup -d /dev/loop0 &> /dev/null

sudo rm -r disk.img fs/

if [[ -f disk.img ]]
then
  echo -e "\e[0;34mFILE EXISTS"

  sudo losetup -o1048576 /dev/loop0 disk.img
else
  echo -e "\e[0;34mMAKING IMAGE"
  dd if=/dev/zero of=disk.img bs=516096c count=1000

  echo -e "\e[0;34mResetting Perms"
  sudo chmod 777 disk.img

  echo -e "\e[0;34mFormatting Disk"
  sudo echo ';' | sfdisk disk.img

  echo -e "\e[0;34mMaking EXT2 Partition"
  sudo losetup -o1048576 /dev/loop0 disk.img
  sudo mke2fs /dev/loop0 #blocks
fi

mkdir fs &> /dev/null
mkdir "fs/DISK NOT MOUNTED" &> /dev/null

echo -e "\e[0;34mMounting disk.img"

sudo mount -text2 /dev/loop0 fs/
sudo chmod 777 -R disk.img fs/

echo "Hello World" > fs/"THIS IS A NEW FILE.txt"
mkdir fs/Hello
echo "test" > fs/Hello/"Wow.txt"