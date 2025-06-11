/* --------------------------------------------------------
                         systick.h

     Header fuer einen Systemticker. Zaehlt im System
     automatisch 32-Bit Zeiteinheiten hoch:

         system_zsec    : Zehntelsekunden
         system_halfsec : togglet jede halbe Sekunde 1-0
         system_sec     : Sekunden

     einfaches Blinkprogramm: Das "Hallo Welt" fuer
     Mikrocontroller, Blinkintervall geschieht durch den
     Systemticker

     MCU : ch32v003

     04.04.2025  R. Seelig
   -------------------------------------------------------- */

/*
 * Example for using SysTick with IRQs
 * 03-25-2023 E. Brombaugh
 * 05-12-2023 C. Lohr (Modified to reflect updated sysclk)
 * 09-25-2024 ADBeta (Minor updates to main loop, comments and added
 *                    convenient macro function)
 * 04-03-2025 R. Seelig (deutsche Kommentare und eingefuegte system_halfsec, system_sec
 *                       system_zsec)
 */

#ifndef _in_systick
  #define _in_systick

  #include "ch32fun.h"

  // Anzahl Takte die fuer eine Millisekunde benoetigt werden (48MHz / 48.000 = 1kHz (1000) => 1 ms)
  #define SYSTICK_ONE_MILLISECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000)

  // Anzahl Takte die fuer eine Mikrosekunde benoetigt werden (48MHz / 48 = 1MHzHz (1000000) => 1 us)
  #define SYSTICK_ONE_MICROSECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000000)

  // Arduino-Style Makros zum Zugriff auf die Millisekunden
  // micros() liest den Systick counter und dividiert diesen durch SYSTIC_ONE_MICROSECOND
  // Hinweis: Mikrosekundenausgabe laeuft alle 90 Sekunden ueber

  #define millis()           (systick_millis)
  #define system_msec        (systick_millis)
  #define micros()           (SysTick->CNT / SYSTICK_ONE_MICROSECOND)

  // Variable, die in SysTick IRQ gezaehlt werden, hier jede Millisekunde
  extern volatile uint32_t systick_millis;
  extern volatile uint32_t system_zsec;
  extern volatile uint32_t system_halfsec;
  extern volatile uint32_t system_sec;

  void systick_init(void);


#endif
