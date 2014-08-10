TARGET           = bin/spoke
BUILD_MCU        = atmega328p
AVRDUDE_MCU      = m328p
F_CPU            = 8000000L

# Internal 8mhz clock
FUSE_L = 0xE2
FUSE_H = 0xD9
FUSE_E = 0xFF

AVR_TOOLS_PATH   = /usr/local/bin
AVRDUDE          = $(AVR_TOOLS_PATH)/avrdude -c usbasp -P usb -p $(AVRDUDE_MCU)

SRCDIR   = src
BUILDDIR = build

DEFS     = -mmcu=$(BUILD_MCU) -DF_CPU=$(F_CPU)
INCS     = -I. \
           -Isrc \
           -I../shared/ \

OPTS      = -Os -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -ffreestanding
CWARN     = -Wstrict-prototypes -Wall
CXXWARN   = -Wall

CFLAGS   = $(DEFS) $(INCS) $(OPTS) $(CWARN)
CXXFLAGS = $(DEFS) $(INCS) $(OPTS) $(CXXWARN)

CXX = $(AVR_TOOLS_PATH)/avr-g++
CC  = $(AVR_TOOLS_PATH)/avr-gcc
AR  = $(AVR_TOOLS_PATH)/avr-ar

C_SOURCES   = $(shell find $(SRCDIR) -type f -name *.c)
CXX_SOURCES = $(shell find $(SRCDIR) -type f -name *.cpp)

CORE_LIB_SRC =
CORE_LIB_CXX_SRC = \
	../shared/SerialIO.cpp \
	../shared/SPI.cpp \
	../shared/EEPROM.cpp \
	../shared/time.cpp \

OBJECTS := $(CORE_LIB_SRC:.c=.o)
OBJECTS += $(CORE_LIB_CXX_SRC:.cpp=.o)
OBJECTS += $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(C_SOURCES:.c=.o))
OBJECTS += $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(CXX_SOURCES:.cpp=.o))

help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build $(TARGET).hex"
	@echo "make elf ....... to build $(TARGET).elf"
	@echo "make epp ....... to build $(TARGET).epp"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware"
	@echo "make clean ..... to delete objects and hex file"
	@echo "make pins ...... to write pins.h"
	@echo "make analyze ... open Logic.app"

program: flash fuse

hex: $(TARGET).hex

elf: $(TARGET).elf

epp: $(TARGET).epp

flash: hex
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i

fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" -a "$(FUSE_E)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L, FUSE_H, FUSE_E!"; exit 1; }
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

$(TARGET).elf: $(OBJECTS)
	@echo " Linking (.elf)..."
	$(CC) $(CFLAGS) -Wl,-gc-sections $^ -o $(TARGET).elf $(LIB) -lm

$(TARGET).eep: $(TARGET).elf
	@echo " Making eeprom file (.epp)..."
	@avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep

$(TARGET).hex: $(TARGET).eep
	 avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	 avr-size $(TARGET).hex

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning...";
	$(RM) -r $(BUILDDIR) $(BINDIR) $(TARGET) $(ARDUINO_CORE)/*.o

pins:
	pin-defines --device=$(BUILD_MCU)

analyze:
	open /Applications/Logic.app

.PHONY: clean