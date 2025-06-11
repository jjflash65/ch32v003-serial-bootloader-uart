############################################################
#
#                         Makefile
#
############################################################

############################################################
#                 Benutzerschnittstelle
#
#   hier kann der verwendete Controller und der
#   AVR-Programmer gewaehlt werden
############################################################

PROJECT    = ardulink

# den verwendeten Conroller "unkommentieren"

#MCU       = attiny2313
#MCU       = atmega8
#MCU       = atmega88
#MCU       = atmega168
MCU       = atmega328p


# fuer AVRDUDE, als PROGRAMMER kann angegeben werden: arduino, usbtiny, usbasp,
#                                                     avrisp, stk500v2
#
# PROGPORT eines originalen Arduino ist ttyACM0, ein China-Clone hat den PROGPORT ttyUSB0
#
# Fuer gewaehlte Programmer usbtiny und usbasp kann die Angabe in PROGPORT und BRATE entfallen


#PROGRAMMER = stk500v2
#PROGRAMMER = avrisp
#PROGRAMMER = usbtiny
#PROGRAMMER = usbasp
PROGRAMMER = arduino

PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B 3


############################################################
#          funktionaler Teil, hier nichts aendern
############################################################

CFLAGS     = -DF_CPU=16000000 -Wall -Wextra -O1 -mmcu=$(MCU)

.PHONY: all clean size flash

all: clean $(PROJECT).elf size

clean:
	rm -f $(PROJECT).o $(PROJECT).elf $(PROJECT).hex $(PROJECT).map

$(PROJECT).elf: *.c *.h
	avr-gcc $(CFLAGS) *.c -o ardulink.elf

size: $(PROJECT).elf
	avr-size -C $(PROJECT).elf --mcu=$(MCU)
	@avr-objcopy -j .text -j .data -O ihex $(PROJECT).elf $(PROJECT).hex

flash:
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PROGPORT) -b $(BRATE) $(DUDEOPTS) -V -U flash:w:$(PROJECT).elf:e

