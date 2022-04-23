---
title: Linux for Embedded Systems -- Laboratory ex. 2
geometry: margin=2.5cm
author:
  - Marcin Wojnarowski, 303880
date: 20-04-2022
---

# Description of the assignment

1. Prepare an application in a compiled language (C or C++ preferred, make sure that the chosen language is supported in BR), which uses the buttons and LED diodes. When reading the buttons, please take into consideration the "_contact bounce_" effect.
2. It is required that the application implements thorough error checking (you can’t assume that e.g., all hardware communication functions always succeed).
3. The application must respond to changes in the state of the buttons without active waiting (should sleep waiting for the required change)
4. The application functionality should be defined by the student: _Programmable LEDs using buttons for action invocation and `stdin` for programming: dioed._
5. The application should be converted into the Buildroot package
6. The student should demonstrate debugging of his application with the gdb debugger
7. Additional task: Prepare the equivalent application in a selected script language (Python? Lua?) and compare both implementations (resource usage - the application itself and the application + required libraries and/or interpreters, event handling time - measured by a system call and saved to a file).

# Procedure to recreate the design from the attached archive

After unpacking the attached archive `wojnarowskim_lab2.tar.gz` there will be a `setup.sh` script. Running it will download buildroot, copy over config files, change the size of the vfat partition, set the `BR2_EXTERNAL` flag for our package, and finally make the image.

# Description of the solution

## GPIO program: dioed

_dioed = diode + led_

Program was written in C using the (libgpiod)[https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/about/] library. The program consists of allowing users to create small programs that will run on the LEDs from the attachment board using buttons and `stdin`. It can run on any other gpio setup, if one was to change the `CHIP`, `button_lines` (input), and `led_lines` (output) variables defined at the beginning of the program in `main.c`. Special care was taken to error handling and possible buffer overflows; that is, after each function call which could potentially fail, a check was added to appropriately react to errors (in this case, this most often ends with graceful program exit). Additionally, safe variants of functions using buffers were used (those were a maximal length is always specified).

`dioed` starts by registering a callback for all gpio inputs (buttons) and idle waits (which can be verified using a simple process monitor such as `htop`: our process sits at 0% CPU when waiting for gpio events). Each of the 4 buttons has an action assigned to them:

1. Starts the programing shell
2. Runs the program
3. Changes frame duration
4. Exits program

Details of the programming language will be skipped, they can be found by running the program and reading the welcome message with instructions.

### contact bounce effect

The contact bounce effect was quite noticeable and had to be dealt with. To do so, a window of rejection was created (`BOUNCE_WINDOW_MS`). We can assume that within some time window a human is be unable to change the input many times. Thus, within this window fluctuations are assumed to be noise and ignored. Since `libgpiod` returns all events with their absolute timestamp, we can easily check for this contact bounce effect.

## Buildroot package

To package `dioed` and make it easily usable from buildroot's menuconfig `BR2_EXTERNAL` was used. It allows to keep external packages in a external directory thus won't be cleaned together with `make clean`. In `mypkg/` the following files can be found:

- `package/`: directory of all external packages, in this case there is only one
  - `dioed/`: my program
    - `Config.in`: Describes the package: name, description, dependencies. Here, `BR2_PACKAGE_LIBGPIOD` is being marked as a dependency.
    - `dioed.mk`: Describes how to build the package. Where the source can be found, license, version, build instructions, and install instructions.
  - `src/`: directory with sources
    - `dioed/`: my program
      - `.clang-format`: formatting configuration
      - `build.sh`: helper script for building cross compiled version using buildroot's toolchain
      - `main.c`: source code of `dioed`
      - `Makefile`: a makefile describing build commands (source files, linking with `libgiod`, compiler, etc)
- `Config.in`: points to available packages
- `external.desc`: name of the external package namespace, helps to the provide absolute paths to our directories
- `external.mk`: points to package `mk` files

To link this package, it is enough to call any `make` command with `BR2_EXTERNAL=path/to/pkgs`, in my case I just used prepare: `make prepare BR2_EXTERNAL=../mypkg/`. Then it can be enabled in menuconfig from the `External options`.

Building the image will then also build my package and include it in the final filesystem.

## Debugging

To debug `dioed` remote debugging had to be used. First, debug info had to be turned on in the compiled packages (using `BR2_ENABLE_DEBUG`), as by default it is disabled. Note: final binaries in the image will be still be stripped of any debug symbols. Then, `gdb` has to be available in our embedded system. However, since it is rather big, we will only include the debug server and debug remotely from the host machine. For that `BR2_PACKAGE_GDB` and `BR2_PACKAGE_GDB_SERVER` were enabled. Then, host `gdb` is enabled with the TUI option in `BR2_PACKAGE_HOST_GDB`.

At this point a gdb server is started on RPi with `gdbserver :9999 dioed` which completely takes over. Then on the host machine we need to locate the gdb which was compiled for the host architecture but targeting aarch64. This can be found in `./output/host/bin/aarch64-buildroot-linux-gnu-gdb`. It needs to be run with our non-stripped version of `dioed` which can be found in `./output/build/dioed-0.1/dioed`. Once done, we can see the debug symbols were read (verifiable with `list main` in the gdb shell) and we can connect to our remote session with `target remote 192.168.145.xxx:9999` in the gdb shell. Finally, we can add breakpoints (for example `bt event_callback`) and inspect variables (`info locals`) after we have ran our program with `continue`/`run`.

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
.
|-- .config
|-- setup.sh
\-- mypkg
  |-- Config.in
  |-- external.desc
  |-- external.mk
  |-- package
  |   \-- dioed
  |       |-- Config.in
  |       \-- dioed.mk
  \-- src
      \-- dioed
          |-- .clang-format
          |-- build.sh
          |-- main.c
          \-- Makefile
```
