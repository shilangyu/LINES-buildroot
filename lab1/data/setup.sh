#!/bin/sh

# get buildroot
wget https://buildroot.org/downloads/buildroot-2022.02.tar.xz
tar -xJf buildroot-2022.02.tar.xz
cd buildroot-2022.02

# copy config
cp ../.config .

# change vfat partition size
# sed board/raspberrypi4-64/genimage-raspberrypi4-64.cfg

# create build
# make
