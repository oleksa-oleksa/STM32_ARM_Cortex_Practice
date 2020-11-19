# Prefix for original toolchain
# https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
TPREFIX ?= ../gcc-arm-none-eabi-4_8-2014q3/bin/

CC=$(TPREFIX)arm-none-eabi-gcc
OBJCOPY=$(TPREFIX)arm-none-eabi-objcopy

# Cortex-M4 with FPU, hardfloat, support thumb
CPUFLAGS=-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb

LDFLAGS=$(CPUFLAGS)
LDFLAGS += -Wl,-T./arm-gcc-link.ld \
	   -Wl,-Map=STM32F415RG.map \
	   -Wl,--gc-sections -g -L./ \
	   -nostartfiles
LDLIBS=-lSTM32F415RG -lm

CFLAGS=$(CPUFLAGS)
CFLAGS += -Wall -g -O0
CFLAGS += -ffunction-sections

INCS=-I./ \
     -isystemcmsis/ \
     -isystemcmsis_lib/include/ \
     -isystemcmsis_boot/ \
     -isystemMPP/BME280/ \
     -isystemMPP/BOARD/ \
     -isystemMPP/CC3100/ \
     -isystemMPP/CC3100/simplelink/include \
     -isystemMPP/CC3100/simplelink/source \
     -isystemMPP/SD_CARD/ \
     -isystemMPP/USB_CDC/ \
     -isystemMPP/DW1000/ \
     -isystemMPP/DW1000/driver/ \
     -isystemMPP/MPU9250/ \
     -isystemMPP/MQTT/ \
     -isystemMPP/MQTT/inc/ \
     -isystemCoOS/ \
     -isystemCoOS/portable/ \
     -isystemCoOS/kernel/
DEFINES=-DSTM32F415RG \
	-DSTM32F4XX \
	-DUSE_STDPERIPH_DRIVER \
	-D__ASSEMBLY__ \
	-D__FPU_USED
CPPFLAGS=$(INCS) $(DEFINES) -MMD -MF $*.d


all: STM32F415RG.bin

# Generate list of all objects
OBJS=main.o aufgabe.o test.o
include MPP/targets.mk
include cmsis_boot/targets.mk
include cmsis_lib/source/targets.mk
include CoOS/targets.mk
-include *.d

STM32F415RG.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
# if dependency list has changed, recompile too
%.o: %.d
# if dependency list doesn't exist, don't panic
%.d: ;

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	@$(RM) -fv $(OBJS) $(OBJS:.o=.d) STM32F415RG.elf STM32F415RG.bin STM32F415RG.map
	
doc: .doxygen
.doxygen: Doxyfile
	doxygen Doxyfile
	touch .doxygen

Doxyfile: Doxyfile.in
	sed '/^INPUT/d;/^OUTPUT_DIRECTORY/d;' Doxyfile.in > Doxyfile

clean-doc:
	$(RM) -r html/ latex/
	@$(RM) .doxygen
	@$(RM) Doxyfile
	
flash: STM32F415RG.bin
	st-flash write $< 0x8000000

.PHONY: all clean doc clean-doc flash
.SUFFIXES:
