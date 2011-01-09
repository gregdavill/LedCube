TARGET         = led_cube
CSRC           = main.c uart.c at45db161d.cpp
ASRC           = xitoa.S
MCU_TARGET     = atmega88pa
OPTIMIZE       = -Os -mcall-prologues
DEFS           =
LIBS           =
DEBUG          = dwarf-2

CC             = avr-gcc
ASFLAGS        = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ALL_ASFLAGS    = -mmcu=$(MCU_TARGET) -I. -x assembler-with-cpp $(ASFLAGS)
CFLAGS         = -Wall -g$(DEBUG) $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
LDFLAGS        = -Wl,-Map,$(TARGET).map
OBJ            = $(CSRC:.c=.o) $(ASRC:.S=.o)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size



all: $(TARGET).elf lst text size

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -rf *.o $(TARGET).elf *.eps *.bak *.a
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf $(TARGET).hex

size: $(TARGET).elf
	$(SIZE) -C --mcu=$(MCU_TARGET) $(TARGET).elf

lst:  $(TARGET).lst
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



text: hex
hex:  $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

prog:
	avrdude -c arduino -p m88 -F -b 115200 -P COM6 -U flash:w:$(TARGET).hex
