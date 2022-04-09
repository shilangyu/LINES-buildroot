---
title: Linux for Embedded Systems -- Laboratory ex. 2
geometry: margin=2.5cm
author:
  - Marcin Wojnarowski, 303880
date: 12-04-2022
---

# Description of the assignment

1. Prepare an application in a compiled language (C or C++ preferred, make sure that the chosen language is supported in BR), which uses the buttons and LED diodes. When reading the buttons, please take into consideration the "_contact bounce_" effect.
2. It is required that the application implements thorough error checking (you can’t assume that e.g., all hardware communication functions always succeed).
3. The application must respond to changes in the state of the buttons without active waiting (should sleep waiting for the required change)
4. The application functionality should be defined by the student (below are just examples):
   1. The timer controlled by the buttons (with lap-time function). LEDs should be used to confirm the operation. The measured time should be displayed on the console.
   2. Coded lock with tampering detection. LEDs should signal the opening of the lock, an erroneous code, alarm state after the certain number of unsuccessful attempts.
   3. "_Memory game_": application displays a sequence using the LEDs. The user must repeat it with the buttons.
   4. Application for simple tests. The questions (read together with the right answers from an external file) are displayed in the console. The user answers with the buttons. After each response, the LEDs signal: good answer, wrong answer, skipped question. At the end, the result is displayed in the console.
   5. Morse code transmitter and receiver. The LED transmits the text sent by the console. The text entered by the switch is decoded and output to the console).
5. The application should be converted into the Buildroot package
6. The student should demonstrate debugging of his application with the gdb debugger
7. Additional task: Prepare the equivalent application in a selected script language (Python? Lua?) and compare both implementations (resource usage - the application itself and the application + required libraries and/or interpreters, event handling time - measured by a system call and saved to a file).

TODO: replace above with my task idea

# Procedure to recreate the design from the attached archive

After unpacking the attached archive `wojnarowskim_lab2.tar.gz` there will be a `setup.sh` script. Running it will download buildroot, copy over config files, change the size of the vfat partition, set the `BR2_EXTERNAL` flag for our package, and finally make the image.

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
7. Set external packages with `make prepare BR2_EXTERNAL=../mypkg/`

# Attached archive

```
.
|-- .config
|-- setup.sh
TODO
```
