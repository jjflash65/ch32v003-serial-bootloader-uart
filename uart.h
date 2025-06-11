/* --------------------------------------------------------
                         uart.h

     Header Softwaremodul fuer serielle Schnittstelle

     TxD an PD5, RxD an PD6

     MCU : ch32v003

     25.03.2025  R. Seelig
   -------------------------------------------------------- */

/*
            +-----------+
  PC1 / sda |1    C   16| PC0
  PC2 / scl |2    H   15| Vdd
        PC3 |3    3   14| gnd
        PC4 |4    2   13| PA2 / osc0 / a0
 PC6 / mosi |5    V   12| PA1 / osc1 / a1
 PC7 / miso |6    0   11| nrst / PD7
 PD1 / swio |7    0   10| PD6 / a6 / urx
   PD4 / a7 |8    3    9| PD5 / a5 / utx
            +-----------+
*/

#ifndef in_uart
  #define in_uart

  #include "ch32fun.h"


  // readint_enable beinhaltet eine Eingabefunktion fuer dezimalen Integer
  // 1=> Funktion verfuegbar; 0=> Funktion nicht verfuegbar
  #define readint_enable     0

  void uart_init(uint32_t baudrate);
  void uart_putchar(uint8_t ch);
  uint8_t uart_getchar(void);
  uint8_t uart_ischar(void);
  #if (readint_enable == 1)
    int16_t readint(void);
  #endif

#endif
