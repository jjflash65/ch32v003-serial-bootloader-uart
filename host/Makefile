###############################################################################
#                                MAKEFILE
#
#  erzeugt wahlweise Bootloader fuer STM8S103 oder STM8S105 oder das
#  Hostprogramm
#
###############################################################################

PROJECT     = v003flash

PHONYY: all clean


all:
	fpc $(PROJECT).pas 1>&2

clean:
	@rm  -f *.ihx
	@rm  -f *.lst
	@rm  -f *.o
	@rm  -f *.ppu
	@rm  -f stm8_bootflash2b
