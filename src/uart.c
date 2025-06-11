/* --------------------------------------------------------
                         uart.c

     Softwaremodul fuer serielle Schnittstelle

     TxD an PD5, RxD an PD6

     MCU : ch32v003

     25.03.2025  R. Seelig
   -------------------------------------------------------- */

#include "uart.h"

/* ----------------------------------------------------------
                           uart_init

     initialisiert serielle Schnittstelle mit Protokoll 8N1
     TxD an Pin PD5, RxD an Pin PD6

     Uebergabe:
        baudrate : einzustellende Baudrate
   ---------------------------------------------------------- */
void uart_init(uint32_t baudrate)
{
  // Enable UART and GPIOD
  RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

  // Push-Pull, 10MHz Output on D5, alternate function (TxD)
  GPIOD->CFGLR = (GPIOD->CFGLR & ~(0x0f << (4*5))) |
                ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<( 4*5 ));

  // Setup UART for 8n1
  USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx | USART_Mode_Rx;
  USART1->CTLR2 = USART_StopBits_1;

  // Set baud rate and enable UART
  USART1->BRR = ((FUNCONF_SYSTEM_CORE_CLOCK) + (baudrate)/2) / (baudrate);
  USART1->CTLR1 |= CTLR1_UE_Set;
}

/* ----------------------------------------------------------
                        uart_putchar

     sendet ein Zeichen auf der seriellen Schnittstelle

     Uebergabe:
       ch : zu sendendes Zeichen
   ---------------------------------------------------------- */
void uart_putchar(uint8_t ch)
{
   while(!(USART1->STATR & USART_FLAG_TXE));            // warten bis evtl. vorangegangenes Zeichen gesendet ist
   USART1->DATAR = ch;
}

/* ----------------------------------------------------------
                        uart_getchar

     wartet, bis auf der seriellen Schnittstelle ein Zeichen
     eingegangen ist und liest dieses ein.

     Rueckgabe:
       gelesenes Zeichen
   ---------------------------------------------------------- */
uint8_t uart_getchar(void)
{
  while(!(USART1->STATR & USART_FLAG_RXNE));
  return USART1->DATAR;
}

/* ----------------------------------------------------------
                        uart_ischar

     testet, ob auf der seriellen Schnittstelle ein Zeichen
     eingegangen ist, liest dieses aber nicht ein

     Rueckgabe:
       1 : es ist ein Zeichen eingetroffen
       0 : kein Zeichen verfuegbar
   ---------------------------------------------------------- */
uint8_t uart_ischar(void)
{
  return (USART1->STATR & USART_FLAG_RXNE);
}

#if (readint_enable == 1)
  /* ------------------------------------------------------------
                              readint

       liest einen 16-Bit signed Integer auf der seriellen
       Schnittstelle ein (allerdings reicht der Eingabebereich
       nur von -32767 .. +32767).

       Korrektur ist mit der Loeschtaste nach links moeglich.
     ------------------------------------------------------------ */
  int16_t readint(void)
  {
    uint8_t   signflag= 0;
    uint16_t  sum = 0;
    uint8_t   zif;
    uint8_t   ch;

    do
    {
      ch= uart_getchar();
      if (ch== 0x0a) ch= 0x0d;

      // Ziffern auswerten
      if ((ch>= '0') && (ch<= '9'))
      {
        zif= ch-'0';

        if ((sum== 0) && (zif))        // erste Ziffer
        {
          sum= zif;
          uart_putchar(ch);
        }
        else
        if (sum < 3277)
        {
          if  (!( ((sum * 10) > 32750) && (zif > 7) ))
          {
            sum= (sum*10) + zif;
            uart_putchar(ch);
          }
        }
      }

      // letzte Eingabe loeschen
      if ((ch== 127) || (ch== 8))    // letzte Eingabe loeschen
      {
        if (sum)
        {
          sum /= 10;
          uart_putchar(8);
          uart_putchar(' ');
          uart_putchar(8);
        }
        else
        if (signflag)
        {
          uart_putchar(8);
          uart_putchar(' ');
          uart_putchar(8);
          signflag= 0;
        }
      }

      // Eingabe Minuszeichen
      if ((ch== '-') && (sum == 0))
      {
        signflag= 1;
          uart_putchar('-');
      }

    } while (ch != 0x0d);              // wiederholen bis Returnzeichen eintrifft
    if (signflag) return sum *(-1); else return sum;
  }

#endif
