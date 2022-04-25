---
title: Linux for Embedded Systems -- Laboratory ex. 3
geometry: margin=2.5cm
author:
  - Marcin Wojnarowski, 303880
date: 25-04-2022
---

# Description of the assignment

1. Prepare the "administrative" Linux system similar to the „rescue” system used in our lab:
   1. working in initramfs
   2. equipped with the tools necessary to manage the SD card (partition it, format it, copy the new version of the system via a network, etc.). Particularly this system will be used to split the SD card into 3 partitions:
      1. VFAT with the rescue system (in „rescue” subdirectory), the administrative system and the kernel of the usr system (in the „user” subdirectory) (in fact it should be the original first partition),
      2. ext4 – with the rootfs of the user system,
      3. ext4 – with the data of the user system.
2. Prepare a "utility" Linux operating system using the ext4 on the 2nd partition as its root file system. This system should provide the web server (implemented with Tornado, or equivalent framework) controlled via WWW interface.
   1. The server should serve files located on the 3rd partition (displaying the list of files and allowing selection of file to download).
   2. The server should also allow the authenticated users to upload new files to the 3rd partition.
3. Prepare the bootloader enabling selecting between the administrative and the user system. Cautions! That bootloader should be started by the original bootloader based in the RPi firmware. It should be run as its user system. Due to limitations of the RPi, our bootloader must pass to the loaded kernel the kernel parameters and the device tree provided by the original bootloader.
   1. The WHITE LED should signal, that the buttons will be checked
   2. After one second, the buttons should be read to select the system. If the chosen button is NOT pressed, the „utility” system should be loaded. If the chosen button is pressed, the „administrative” system should be loaded.
   3. After selection of the system, the WHITE led should be switched off. The GREEN LED should be ON if the „utility” system was selected. The RED LED should be ON if the „administrative” system was selected.

# Procedure to recreate the design from the attached archive

After unpacking the attached archive `wojnarowskim_lab3.tar.gz` there will be a `setup.sh` script. Running it will download buildroot, copy over config files, change the size of the vfat partition, TODO, and finally make the image.

# Description of the solution

TODO

# Description of the modifications of BR and kernel configurations

Buildroot configuration:

1. TTY port - `console`
2. Primary download site - `192.168.137.24/dl`
3. Select the toolchain - external, AArch64 2021.07
4. Enable `CCACHE`, `BR2_CCACHE_DIR` set to `../ccache-br`
5. Filesystem images set to "initial RAM filesystem linked into linux kernel" with ext4, size increased to 256M (both here and in `board/raspberrypi4-64/genimage-raspberrypi4-64.cfg`)
6. Set gzip compression for image
7. Enable debug info with `BR2_ENABLE_DEBUG`
8. Enable libgiod with `BR2_PACKAGE_LIBGPIOD`
9. Set external packages with `make prepare BR2_EXTERNAL=../mypkg/`
10. Enable `BR2_PACKAGE_DIOED`
11. Enable gdb server with `BR2_PACKAGE_GDB_SERVER` and `BR2_PACKAGE_GDB`
12. Enable host gdb with `BR2_PACKAGE_HOST_GDB`

# Attached archive

```
TODO
```
