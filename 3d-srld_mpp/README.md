# 3D SRLD MPP

## Download toolchain

Download the toolchain for your respective platform from

* Linux:
  https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
* Windows (untested):
  https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-win32.zip
* macOS (untested):
  https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-mac.tar.bz2

and extract it **next** to this source directory, i.e.:

    $ ls 
    gcc-arm-none-eabi-4_8-2014q3
    3d-srld-mpp

Alternatively, edit the path(s) in the `CMakeLists.txt` or `Makefile`, depending
on which build system you intend to choose.

Also, install the STLink tools in order to flash, e.g.,

    $ sudo apt-get install stlink-tools

If you want to build the documentation, install Doxygen (and LaTeX for the PDF).

## Building

This project supports CMake as well as "pure" GNU/Make, and the former is widely
supported by IDEs as well (if you are so inclined).

### CMake with CLion

In order to use this project from within CLion, use the CMake build system:

1. Open CLion and select "Open/Import project".
2. Navigate to this directory
3. Select "CMake Project"

CLion will now run configure and run CMake.  You can build the binaries, flash
the board (with the `st-link` util) as well as the documentation (found in the
`doc/` subdirectory of the build dir.

### CMake on the console

You can also use CMake on the console:

1. Create a new build directory within this directory, e.g. `mkdir build`; then `cd` into it.
2. Configure CMake by pointing it to the source tree, e.g. `cmake ..`
3. You can now run `make` to build the binaries, `make flash` to flash and
   `make doc` to build the documentation as described in "CMake with CLion"

### pure GNU/make (Linux and macOS only)

Alternatively you can simply run `make` from the source tree which will not
use generated Makefiles but hand-written ones.  As before, you can run
`make`, `make flash` and `make doc`.

## Debugging

In order to debug on the board you need to use the GDB server provided by the
stlink-tools which sets up communication between the board and the GNU debugger
(GDB).

First, start the debug server in one terminal session

    $ st-util

This will now run in background and connect with the device via the STLink
interface.  Open another terminal session and start the packaged GDB from the
toolchain and pass the `.elf` file you built, e.g., for GNU/Make from the
**project directory**:

    $ ../gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gdb STM32F415RG.elf

And for CMake from the **build directory**:

    $ ../../gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gdb STM32F415RG.elf

This will startup GDB loading the binary from your disk.  Now connect it to
the STLink debug server from within the GDB shell:

    (gdb) target extended localhost:4242

You can now use `load` from GDB to load the current state, use `continue` to
continue execution, etc.  You can find more information on that here:
https://github.com/stlink-org/stlink/blob/develop/doc/tutorial.md
