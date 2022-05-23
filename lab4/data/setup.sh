#!/bin/sh

set -e

# get buildroot
wget https://buildroot.org/downloads/buildroot-2022.02.tar.xz
tar -xJf buildroot-2022.02.tar.xz

# copy config
cp .config buildroot-2022.02/.config

# change vfat partition size
sed -i.bak 's/32M/256M/g' buildroot-2022.02/board/raspberrypi4-64/genimage-raspberrypi4-64.cfg

# create images
cd buildroot-2022.02
make
