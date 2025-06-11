TARGET_MCU        = CH32V003

TARGET            = $(PROJECT)

PREFIX            = riscv-none-elf

NEWLIB            = /usr/include/newlib

INCLUDE_PATH      = $(INC_DIR)
ifeq ($(INC_DIR),)
	INCLUDE_PATH = -I./ -I../include
endif

CH32FUN           = $(dir $(lastword $(MAKEFILE_LIST)))

TARGET_EXT        = c

CH32FUN           = $(dir $(lastword $(MAKEFILE_LIST)))

SYSTEM_C          = ./ch32fun/ch32fun.c

ifeq ($(DEBUG),1)
	EXTRA_CFLAGS   += -DFUNCONF_DEBUG=1
endif

CFLAGS            = -g -Os -flto -ffunction-sections -fdata-sections -fmessage-length=0 -msmall-data-limit=8
LDFLAGS          += -Wl,--print-memory-usage -Wl,-Map=$(TARGET).map

GCCVERSION13      = $(shell expr `$(PREFIX)-gcc -dumpversion | cut -f1 -d.` \>= 13)

MCU_PACKAGE       = 1
GENERATED_LD_FILE = $(CH32FUN)/generated_ch32v003.ld
LDFLAGS          += -L$(CH32FUN)/../misc -lgcc

CFLAGS_ARCH      += -march=rv32ec -mabi=ilp32e

CFLAGS_ARCH      += -D$(TARGET_MCU)

TARGET_MCU_LD     = 0

LDFLAGS          += -lgcc

GENERATED_LD_FILE = $(CH32FUN)/generated_$(TARGET_MCU_PACKAGE)_$(TARGET_MCU_MEMORY_SPLIT).ld

CFLAGS           += \
	$(CFLAGS_ARCH) -static-libgcc \
	-I$(NEWLIB) \
	-I$(CH32FUN)/../extralibs \
	-I$(CH32FUN) \
	-nostdlib \
	-I. -Wall $(EXTRA_CFLAGS) \
	$(INCLUDE_PATH)

LDFLAGS          += -T $(LINKER_SCRIPT) -Wl,--gc-sections
FILES_TO_COMPILE  = $(SYSTEM_C) $(TARGET).$(TARGET_EXT) $(ADDITIONAL_C_FILES) $(SRC)

$(TARGET).bin : $(TARGET).elf
	$(PREFIX)-objdump -S $^ > $(TARGET).lst
	$(PREFIX)-objcopy $(OBJCOPY_FLAGS) -O binary $< $(TARGET).bin
	$(PREFIX)-objcopy -O ihex $< $(TARGET).hex

closechlink :
	-killall minichlink

terminal : monitor

monitor :
	minichlink -T

unbrick :
	minichlink -u

gdbserver :
	-minichlink -baG

gdbclient :
	gdb-multiarch $(TARGET).elf -ex "target remote :3333"


.PHONY : $(GENERATED_LD_FILE)

$(GENERATED_LD_FILE) :
	$(PREFIX)-gcc -E -P -x c -DTARGET_MCU=$(TARGET_MCU) -DMCU_PACKAGE=$(MCU_PACKAGE) -DTARGET_MCU_LD=$(TARGET_MCU_LD) -DTARGET_MCU_MEMORY_SPLIT=$(TARGET_MCU_MEMORY_SPLIT) $(CH32FUN)/ch32fun.ld > $(GENERATED_LD_FILE)

$(TARGET).elf : $(FILES_TO_COMPILE) $(LINKER_SCRIPT) $(EXTRA_ELF_DEPENDENCIES)
	$(PREFIX)-gcc -o $@ $(FILES_TO_COMPILE) $(CFLAGS) $(LDFLAGS) 1>&2

# Rule for independently building ch32fun.o indirectly, instead of recompiling it from source every time.
# Not used in the default 003fun toolchain, but used in more sophisticated toolchains.

ch32fun.o : $(SYSTEM_C)
	$(PREFIX)-gcc -c -o $@ $(SYSTEM_C) $(CFLAGS)

flash :

ifeq ($(USE_ARDULINK),1)
	./minichlink/minichlink -c $(PROGPORT) -C ardulink -w $(TARGET).bin $(WRITE_SECTION) -b
	./minichlink/minichlink -c $(PROGPORT) -C ardulink -w +a55aff00 option # Enable bootloader, disable RESET

else
	./minichlink/minichlink -w $(TARGET).bin $(WRITE_SECTION) -b
	./minichlink/minichlink -w +a55aff00 option # Enable bootloader, disable RESET
endif

cv_clean :
	rm -rf $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex $(GENERATED_LD_FILE) || true

clean : cv_clean

build : $(TARGET).bin

all : build
