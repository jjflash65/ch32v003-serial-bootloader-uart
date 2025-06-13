############################################################
#
#                         Makefile
#
############################################################

PROJECT         = uart_bootload_page

# hier alle zusaetzlichen Softwaremodule angegeben
SRC             = ../src/uart.c

INC_DIR         = -I ./ -I ../include -I ../ch32fun/

WRITE_SECTION   = bootloader

LINKER_SCRIPT   = ./ch32v003_uart_bootloader.ld

# Programmerauswahl
# fuer USE_ARDULINK = 1 wird der Arduinoprogrammer verwendet, in diesem
#                       Fall muss zusaetzlich der serielle Port angegeben werden
# fuer USE_ARDULINK = 0 wird WCH-LINKE verwendet
#                       serielle Portangabe wird hier ignoriert

PROGPORT      = /dev/ttyUSB0
USE_ARDULINK  = 0

include ./Makefile.boot.mk

ifeq ($(USE_ARDULINK),0)
optionbytes :
	minichlink -w +a55aff00 option # Enable bootloader, disable RESET
	minichlink -w activate_boot.bin flash -b
else ifeq ($(USE_ARDULINK),1)
optionbytes :
	minichlink -c $(progport) -C ardulink -w +a55aff00 option # Enable bootloader, disable RESET
	minichlink -c $(progport) -C ardulink -w activate_boot.bin flash -b
endif
