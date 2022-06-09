---
title: Linux for Embedded Systems -- Laboratory ex. 5
geometry: margin=2.5cm
author:
  - Marcin Wojnarowski, 303880
date: 31-05-2022
---

# Description of the assignment

The subject of assignment 5 is to port assignment 4 implementation to OpenWRT.

# Procedure to recreate the design from the attached archive

After unpacking the attached archive `wojnarowskim_lab5.tar.gz` one can find the overlay needed. There will be no setup script. Many of the steps here require manual which will be described below.

# Description of the solution

## Installing OpenWRT

On the rescue system of the RPi we first download the OpenWRT image and unpack it. Luckily the rescue system contains the tool `losetup` which will be used to setup loop devices. The initramfs has sufficient space for us to unpack the image right there. So we proceed and create the loopback devices after which we get two new loopback devices: one for the image and other for the rootfs. In my case those were created under `/dev/loop1p1` and `/dev/loop1p2` respectively. Next we mount the vfat partitions. I tend to mount things under `/mnt` so I mounted them there. The vfat partition of the RPi was mounted under `/mnt/vfat` and the vfat partition of the OWRT image under `/mnt/owrt`. Now from the owrt partition we copy over 3 important files to the vfat partition of the RPi (under `/user`): device table, cmdline.txt, and the image itself. Finally, we write the rootfs to RPi's second partition (thus it has be before prepared by for example using `fdisk` pointing to RPi device). It was made sure that the second partition indeed is big enough. The filesystem image was written using `dd`.

Here is a script that does the things described above, ran on the rescue system of RPi:

\tiny

```sh
# donwload openwrt
wget https://downloads.openwrt.org/releases/22.03.0-rc1/targets/bcm27xx/bcm2711/openwrt-22.03.0-rc1-bcm27xx-bcm2711-rpi-4-ext4-factory.img.gz

# unpack
gzip -d openwrt-22.03.0-rc1-bcm27xx-bcm2711-rpi-4-ext4-factory.img.gz

# create loop devices
losetup -fP openwrt-22.03.0-rc1-bcm27xx-bcm2711-rpi-4-ext4-factory.img

# create mount places
mkdir /mnt/{vfat,owrt}

# mount vfat partitions
mount /dev/mmcblk0p1 /mnt/vfat
mount /dev/loop1p1 /mnt/owrt

# copy over openwrt vfat files
cp /mnt/owrt/kernel8.img /mnt/vfat/user/Image
cp /mnt/owrt/bcm2711-rpi-4-b.dtb /mnt/vfat/user
cp /mnt/owrt/cmdline.txt /mnt/vfat/user

# write the filesystem image
dd if=/dev/loop1p2 of=/dev/mmcblk0p2 bs=4096
```

\normalsize

As a last step we unmount all partitions and sync fs changes.

## Network configuration for OpenWRT

Now that the OpenWRT is installed, we need to reconfigure its network configuration to avoid it taking over the school local network. To do this, the RPi was unplugged from the network (removal of the ethernet cable) and then we booted into OpenWRT (by simply restarting the system, OpenWRT is default if no buttons are pressed on the RPi extension board). In `/etc/config/network` the following change was made:

```diff
@@ -1,5 +1,3 @@
 config interface 'lan'
-    option type 'bridge'
     option ifname 'eth0'
-    option proto 'static'
-    option ipaddr '192.168.1.1' option netmask '255.255.255.0' option ip6assign '60'
+    option proto 'dhcp'
```

Then we restart the network with `/etc/init.d/network reload` and remove the DNS/DHCP server with `rm /etc/rc.d/S19dnsmasq`.

## Configuration of user application

Now that the system is ready, we reboot into it with the ethernet cable plugged in. Every needed dependency for the application from lab4 can be installed using OpenWRT's package manager: `opkg`.

To start, an update of packages has to be downloaded. Then I proceed to install python, libgpiod, and flask. No more packages will be needed, dependencies will be installed automatically.

```sh
# update packages repo
opkg update

# install needed packages
opkg python3 python3-flask libgpiod
```

In the attached archive one can find the application. It has to be placed under `/opt/file-server`.

Finally, to use it as a daemon on OpenWRT, procd is used. To do that, a simple script is created and placed under `/etc/init.d` as `file-server`, and marked as executable with `chmod +x`:

```sh
#!/bin/sh /etc/rc.common

USE_PROCD=1

START=99
STOP=01

start_service() {
    procd_open_instance
    procd_set_param command /bin/sh -c "python3 /opt/file-server/main.py"
    procd_set_param stdout 1
    procd_set_param stderr 1
    procd_close_instance
}
```

We enable procd, set its priority as lowest (99, to make sure every other service is ready) and stop as something, but it won't matter as it won't run on stop. The command to be run is simply running python against our application entry file.

Everything is now ready and we can simply enable this service using `/etc/init.d/file-server enable`, this will ensure the service is started on startup.

Finally, we restart the RPi, reboot into OpenWRT and we can see the application running successfully, by for example visiting `http://<local-ip>:5000`. Of course, it is assumed that the hardware was prepared as in described in lab4 (external IO device was used).

# Attached archive

```
.
`-- fs
    |-- etc
    |   `-- init.d
    |       `-- file-server
    `-- opt
        `-- file-server
            |-- Pipfile
            |-- Pipfile.lock
            |-- gpio.py
            |-- main.py
            |-- templates
            |   `-- index.html
            `-- util.py
```
