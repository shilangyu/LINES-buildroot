#!/bin/sh

set -e

outDir=to-host

rm -rf $outDir
mkdir $outDir

cp boot.txt $outDir
cp buildroot-2022.02/output/images/Image $outDir/Image.admin
cp buildroot-2022.02/output/images/u-boot.bin $outDir
cp buildroot-2022.02-user/output/images/Image $outDir/Image.user
cp buildroot-2022.02-user/output/images/rootfs.ext4.gz $outDir/rootfs.ext4.gz

cd $outDir

python -m http.server

# on rpi

# wget 192.168.145.102:8000/Image.admin
# wget 192.168.145.102:8000/Image.user
# wget 192.168.145.102:8000/u-boot.bin
# wget 192.168.145.102:8000/boot.txt
# mkimage -T script -C none -n 'Start script' -d boot.txt boot.scr
# wget 192.168.145.102:8000/rootfs.ext4.gz -O- | gzip -d | dd of=/dev/mmcblk0p2 bs=4096
