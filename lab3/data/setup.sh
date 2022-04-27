#!/bin/sh

set -e

# get buildroot
wget https://buildroot.org/downloads/buildroot-2022.02.tar.xz
tar -xJf buildroot-2022.02.tar.xz

cp -r buildroot-2022.02 buildroot-2022.02-user

# copy configs
cp .config.admin buildroot-2022.02/.config
cp .config.user buildroot-2022.02-user/.config

# change vfat partition size
sed -i.bak 's/32M/256M/g' buildroot-2022.02/board/raspberrypi4-64/genimage-raspberrypi4-64.cfg
sed -i.bak 's/32M/256M/g' buildroot-2022.02-user/board/raspberrypi4-64/genimage-raspberrypi4-64.cfg

# create images
cd buildroot-2022.02
make
cd ../buildroot-2022.02-user
make
