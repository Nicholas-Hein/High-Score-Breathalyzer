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
LIBDIR := lib
TARGET := $(BINDIR)/$(PROJECTNAME)

# Compiler
CC := avr-gcc
SIZE := avr-size
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
AVR_PROGRAMMER := -c avrdude -P usb
INC := -I"include"
LIB := -B $(LIBDIR) -B $(BINDIR)
CFLAGS :=

# Compiler Optimization
# 	Select the microcontroller
CFLAGS += -mmcu=$(AVR)
#	Select the standard library
CFLAGS += -std=c99
#	Specify the language
CFLAGS += -x c
#	No debugging options
CFLAGS += -g
#	Compile and assemble but do not link
CFLAGS += -c
#	Add includes
CFLAGS += $(INC)
#	Add bytecode
CFLAGS += $(LIB)
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
#	Generate make dependencies and compile
CFLAGS += -MD
#	Generate phony targets for all headers
CFLAGS += -MP


# Configuration Macros
# 	Define the debug symbol
CFLAGSDEBUG := $(CFLAGS) -DDEBUG


# Linker Flags
LDFLAGS := -lc -mmcu=$(AVR)
#	Garbage collect unused sections
LDFLAGS += -Wl,--gc-sections
#	Print a linker map to stdout
LDFLAGS += -Wl,-Map=$(TARGET).map
#	Print garbage collected sections
LDFLAGS += -Wl,--print-gc-sections
#	Performs global optimization
LDFLAGS += -Wl,--relax
#	Correct circular includes (Start)
LDFLAGS += -Wl,--start-group
#	Add libm
LDFLAGS += -Wl,-lm
#	Correct circular includes (End)
LDFLAGS += -Wl,--end-group
#	Add bytecode
LDFLAGS += $(LIB)


CP1FLAGS := -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
CP2FLAGS := -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex
ODFLAGS := -h -S
CP3FLAGS := -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures
SZFLAGS :=

# Files
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))




./$(BUILDDIR)/%.o: ./$(SRCDIR)/%.c
	@echo " Building file: '$<'..."
	$(CC) $(CFLAGS) -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"
	@echo "   Finished building '$<'!"

$(TARGET).elf: $(OBJECTS)
	@echo " Building target: '$@'..."
	$(CC) $(LDFLAGS) -o$(TARGET).elf $(OBJECTS)
	@echo " Finished building target: '$@'!"
	@echo " Dumping..."
	$(OBJCOPY) $(CP1FLAGS) "$(TARGET).elf" "$(TARGET).hex"
	$(OBJCOPY) $(CP2FLAGS) "$(TARGET).elf" "$(TARGET).eep" || exit 0
	$(OBJDUMP) $(ODFLAGS) "$(TARGET).elf" > "$(TARGET).lss"
	$(OBJCOPY) $(CP3FLAGS) "$(TARGET).elf" "$(TARGET).srec"
	$(SIZE) "$(TARGET).elf"
	@echo " Done dumping!"

all: $(TARGET).elf

program: $(TARGET).hex
	@echo " Programming...";
	@echo "   avrdude $(AVR_PROGRAMMER) -p $(AVR) $(LIB) -e -U flash:w:$(TARGET)-o.hex"; avrdude $(AVR_PROGRAMMER) -p $(AVR) $(LIB) -e -U flash:w:$(TARGET)-o.hex

fuses:
	@echo " Testing fuses...";
	@echo "   avrdude $(AVR_PROGRAMMER) -p $(AVR) -B 100 -U lfuse:w:0xe2:m -U hfuse:w:0xd4:m"; avrdude $(AVR_PROGRAMMER) -p $(AVR) -B 100 -U lfuse:w:0xe2:m -U hfuse:w:0xd4:m

clean:
	@echo " Cleaning..."
	$(RM) -r $(BUILDDIR)/* $(BINDIR)/*
	@echo " Done cleaning!"

.PHONY: clean
.PHONY: all
.PHONY: fuses
.PHONY: program
