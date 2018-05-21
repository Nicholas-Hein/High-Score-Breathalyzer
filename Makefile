###
###	HIGH SCORE BREATHALYZER - Makefile
###	Author: Nicholas Hein
###

# Program Details
PROJECTNAME := HighScoreBreathalyzer

# Circuit Details
AVR := atmega8

# Directories
MAINNAME := main
SRCDIR := src
BUILDDIR := build
BINDIR := $(AVR)
TARGET := $(BINDIR)/$(PROJECTNAME)

# Compiler
CC := avr-gcc
SIZE := avr-size
AVR_PROGRAMMER := -c avrdude -P usb
INC := -I include
LIB := -B lib
CFLAGS := -mmcu=$(AVR) -std=c99 -g

# Compiler Optimization
#	Warnings and errors
CFLAGS += -Wall -pedantic-errors -Werror
#	Make function calls efficient
CFLAGS += -mcall-prologues
#	Optimize for size
CFLAGS += -Os
#	Garbage collection
CFLAGS += -fdata-sections -ffunction-sections
#	Default variable size
CFLAGS += -funsigned-char -funsigned-bitfields
#	Pack structure members together
CFLAGS += -fpack-struct
#	Allocate only necessary memory for enums
CFLAGS += -fshort-enums
#	Limit size of functions inlined
CFLAGS += -finline-limit=3

# Configuration Macros
#CFLAGS += -D DEBUG

#Linker Flags
LDFLAGS := -lc -mmcu=$(AVR)
#	Garbage collect unused sections
LDFLAGS += -Wl,--gc-sections
#	Generate MAP file
LDFLAGS += -Wl,-Map,$(PROJECTNAME).map
#	Print garbage collected sections
LDFLAGS += -Wl,--print-gc-sections
#	Performs global optimization
LDFLAGS += -Wl,--relax

# Files
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))



$(TARGET)-o.hex : $(OBJECTS)
	@echo " Building...";
	@echo "   $(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)-o.elf"; $(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)-o.elf
	@echo " Converting objects...";
	@echo "   avr-objcopy -O ihex $(TARGET)-o.elf $(TARGET)-o.hex"; avr-objcopy -O ihex $(TARGET)-o.elf $(TARGET)-o.hex
	@echo " Getting size...";
	@echo "   $(SIZE) --format=berkeley $(TARGET)-o.elf"; $(SIZE) --format=berkeley $(TARGET)-o.elf
	@echo "   $(SIZE) --format=avr --mcu=$(AVR) $(TARGET)-o.elf"; $(SIZE) --format=avr --mcu=$(AVR) $(TARGET)-o.elf

$(TARGET).hex: $(SOURCES)
	@echo " Building...";
	@echo "   $(CC) $(CFLAGS) --combine -fwhole-program $(LDFLAGS) $(SOURCES) -o $(TARGET).elf"; $(CC) $(CFLAGS) --combine -fwhole-program $(LDFLAGS) $(SOURCES) -o $(TARGET).elf
	@echo "Converting objects...";
	@echo "   avr-objcopy -O ihex $(TARGET).elf $(TARGET).hex"; avr-objcopy -O ihex $(TARGET).elf $(TARGET).hex
	@echo " Getting size...";
	@echo "   $(SIZE) --format=berkeley $(TARGET).elf"; $(SIZE) --format=berkeley $(TARGET).elf
	@echo "   $(SIZE) --format=avr --mcu=$(AVR) $(TARGET).elf"; $(SIZE) --format=avr --mcu=$(AVR) $(TARGET).elf

all: $(TARGET).hex

program: $(TARGET)-o.hex
	@echo " Compiling...";
	@echo "   avrdude $(AVR_PROGRAMMER) -p $(AVR) $(LIB) -e -U flash:w:$(TARGET)-o.hex"; avrdude $(AVR_PROGRAMMER) -p $(AVR) $(LIB) -e -U flash:w:$(TARGET)-o.hex

fuses:
	@echo " Testing fuses...";
	@echo "   avrdude $(AVR_PROGRAMMER) -p $(AVR) -B 100 -U lfuse:w:0xe2:m -U hfuse:w:0xd4:m"; avrdude $(AVR_PROGRAMMER) -p $(AVR) -B 100 -U lfuse:w:0xe2:m -U hfuse:w:0xd4:m

clean:
	@echo " Cleaning..."
	@echo "   $(RM) -r $(BUILDDIR)/* $(BINDIR)/* $(BUILDTESTDIR)/*"; $(RM) -r $(BUILDDIR)/* $(BINDIR)/* $(BUILDTESTDIR)/*

.PHONY: clean
.PHONY: all
.PHONY: fuses
