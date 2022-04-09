---
title: Linux for Embedded Systems -- Laboratory ex. 1
geometry: margin=2.5cm
author:
  - Marcin Wojnarowski, 303880
date: 26-03-2022
---

# Description of the assignment

During the first lab the task was to get familiar with basic Build Root (BR) functionalities. We operated on Arch linux compiling a kernel for Raspberry Pi 4 arm64 (RPi). The linux system was compiled with initramfs as the root filesystem.

The following tasks were to be completed:

1. test the possibilities to transfer files between the RPi and the workstation (in both directions)
2. test the possibilities to use pipes to unpack "on the fly" the files transferred via network (it will be useful when installing the user system with rootfs stores in a dedicated SD card partition)
3. RPi should automatically connect to the network, using DHCP to obtain the network parameters. If the network cable is not connected, the connection should be automatically established after connection of the cable. Similarly - disconnection of the cable should shut down the network connection
4. The system host name should be set to `firstname_lastname` of the student
5. When the Internet connection is established, the system clock should be synchronized with an NTP server (preferably using the "NTP server pool")
6. Add the Lua interpreter with the script that after the start of the system tries to download from certain URL `update.tar.gz` file and unpacks it overwriting the filesystem
7. Except of the `root` user, there should be a plain user (with arbitrarily chosen name) account created. Both users should have set the passwords

# Procedure to recreate the design from the attached archive

After unpacking the attached archive `archive.tar.gz` there will be a `setup.sh` script. Running it will download buildroot, copy over config files, change the size of the vfat partition and finally make the image. There might be some problems (such as absolute paths or wrong file permissions) though I did my best to make it as self contained as possible.

# Description of the solution

## 1. & 2.

Using the already available "rescue mode" system we could complete this step:

### On host's side

- Create a dummy archive: `tar -czvf arch.tar.gz folder/`
- Start a simple http server: `python3 -m http.server`

### On RPi's side

- Download hosted archive and decompress on the fly: `wget 192.168.145.xxx:8000/arch.tar.gz -qO- | tar -xzv`

## 3.

The `netplug` package was enabled. After that, the requirement was fulfilled and it could be observed by unplugging and plugging the ethernet cable back in.

## 4.

In `menuconfig` the hostname was changed to `marcin_wojnarowski` using the `BR_TARGET_GENERIC_HOSTNAME` option. To check whether the change was successful we compile the image and put it on RPi. After booting we were greeted with the said hostname and checking `/etc/hostname` also shows `marcin_wojnarowski` proving the correct setup.

## 5.

The ntp package was enabled (`BR2_PACKAGE_NTP`) and its daemon (`BR2_PACKAGE_NTP_NTPD`).

## 6.

First, the Lua interpreter was enabled in _Interpreter languages and scripting_ (`BR2_PACKAGE_LUA`). Then an overlay was set up with `BR2_ROOTFS_OVERLAY` set to `../overlay`. There two files were created:

1. `etc/init.d/S50luascript` -- As we know, executables under `/etc/init.d/S*` are executed on startup and shutdown, thus there a simple middleware script was added to call our lua script. The `S50` prefix is so that it runs after the network setup (scripts are ran in alphabetical order). Additionally, the script has a simple condition to make sure it is ran only on startup, not on shutdown. Of course, it was marked executable with `chmod`.

```sh
#!/bin/sh

[ "$1" = "start" ] && lua /opt/autostart.lua
```

2. `opt/autostart.lua` -- Simple script which does what the task asked us to do: downloads some archive from a hardcoded location and unpacks it.

```lua
os.execute("wget http://192.168.145.97:8000/update.tar.gz -qO- | gzip -d | tar -x")
```

When booting the RPi we could find the unpacked archive under `/` showing the correctness of the setup.

## 7.

Using `BR2_ROOTFS_USERS_TABLES`, it was set to a path `../mkusers.txt`. A file was created at that path and an example user was created named `wojnarowskim`. Then for the root user, `BR2_TARGET_GENERIC_ROOT_PASSWD` was set to some password, in our case it was set to `root`.

# Description of the modifications of BR and kernel configurations

Buildroot configuration:

1. TTY port - `console`
2. Primary download site - `192.168.137.24/dl`
3. Select the toolchain - external, AArch64 2021.07
4. Enable `CCACHE`, `BR2_CCACHE_DIR` set to `/malina/wojnarowskim/ccache-br`
5. Filesystem images set to "initial RAM filesystem linked into linux kernel" with ext4, size increased to 256M (both here and in `board/raspberrypi4-64/genimage-raspberrypi4-64.cfg`)
6. Set gzip compression for image
7. Set `BR2_ROOTFS_OVERLAY` to `../overlay` (to enable adding my own scripts)
8. Enable Lua - `BR2_PACKAGE_LUA`=y (to enable running scripts written in Lua)

...and more, all can be found in the attached `.config`.

# Attachment archive

```
.
|-- .config
|-- mkusers.txt
|-- setup.sh
\-- overlay
    |-- etc
    |   \-- init.d
    |       \-- S50luascript
    \-- opt
        \-- autostart.lua
```
