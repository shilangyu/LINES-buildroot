#!/bin/sh

(
export PATH=$BRPATH/output/host/usr/bin:$PATH
make ARCH=arm CROSS_COMPILE=aarch64-none-linux-gnu- dioed
)
