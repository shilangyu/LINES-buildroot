#!/bin/sh

# get buildroot
wget https://buildroot.org/downloads/buildroot-2022.02.tar.xz
tar -xJf buildroot-2022.02.tar.xz
cd buildroot-2022.02

# copy config
cp ../.config .

# change vfat partition size
sed -i.bak 's/32M/256M/g' board/raspberrypi4-64/genimage-raspberrypi4-64.cfg

# link external package
make prepare BR2_EXTERNAL=../mypkg/

# create build
make
