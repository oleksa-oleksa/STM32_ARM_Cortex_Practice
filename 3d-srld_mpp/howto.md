# How to use Putty to get log output

Run the follwing to open the terminal:
```bash
sudo putty /dev/ttyUSB0 -serial -sercfg <baud-rate>,8,n,1,N
```

The exercise sheet 4 shows how USART2 can be configured:
From usart2_init.c:
baud rate: 921600 
word length: 8
flow control: None

Therefore the following command opens a terminal that shows the log output correctly:

```bash
sudo putty /dev/ttyUSB0 -serial -sercfg 921600,8,n,1,N
```

# How to debug with gdb

Leo has a documentation on how to debug, but he uses `st-utils` which does not support breakpoints in our case.
Therefore, we are using openocd.

To work with arm-none-eabi-gdb, we have to start openocd first.
Follow these steps:

1. find `stm32f4discovery.cfg` (for fedora):
```bash
>locate openocd
...
/usr/share/openocd/scripts/board/stm32f4discovery.cfg
...
``` 

2. run openocd:
```bash
> openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg
Open On-Chip Debugger 0.10.0
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
adapter speed: 2000 kHz
adapter_nsrst_delay: 100
none separate
srst_only separate srst_nogate srst_open_drain connect_deassert_srst
Info : Unable to match requested speed 2000 kHz, using 1800 kHz
Info : Unable to match requested speed 2000 kHz, using 1800 kHz
Info : clock speed 1800 kHz
Info : STLINK v2 JTAG v31 API v2 SWIM v7 VID 0x0483 PID 0x3748
Info : using stlink api v2
Info : Target voltage: 3.238421
Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
```

After that, use the gdb:
1. open gdb (for using cmake in build/):
```bash
../../gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gdb STM32F415RG.elf
```
2. connect to board
```bash
(gdb) target extended localhost:3333
```
3. optional: set breakpoints like so
```bash
(gdb) break filename.c:line
```
4. (I had to do it) load and start programm:
```bash
(gdb) load
...
(gdb) continue
```