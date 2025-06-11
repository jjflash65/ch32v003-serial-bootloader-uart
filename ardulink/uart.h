#ifndef UART_H
#define UART_H

#include <stdio.h>
extern FILE *uart;

/* --------------------------------------------------------
     Registernamen von ATmega8, 8515, ATtiny2313 und 4313
     anpassen

     10.04.2025 R. Seelig
   -------------------------------------------------------- */
#if defined __AVR_ATmega8__    || __AVR_ATmega8515__  ||                                            \
            __AVR_ATtiny2313__ || __AVR_ATtiny4313__

  #define UBRR0       UBRR
  #define UBRR0L      UBRRL
  #define UBRR0H      UBRRH
  #define UCSR0A      UCSRA
  #define UCSR0B      UCSRB
  #define UCSR0C      UCSRC
  #define U2X0        U2X
  #define RXEN0       RXEN
  #define TXEN0       TXEN
  #define UCSZ00      UCSZ0
  #define UCSZ01      UCSZ1
  #define UDRE0       UDRE
  #define UDR0        UDR
  #define RXC0        RXC

#endif


void uart_init();

#endif
