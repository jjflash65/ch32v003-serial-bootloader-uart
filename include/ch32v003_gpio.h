/* -----------------------------------------------
                  ch32v003_gpio.h

     Makros und Defines zum einfachen Umgang mit
     GPIO - Portpins.

     Setzt auf die Deklarationen fun ch32v003
     auf

     25.03.2025  R. Seelig
   ----------------------------------------------- */

/*

                  CH32V003 A4M6                                        CH32V003F4P6

                  +-----------+                                      +--------------+
        PC1 / sda |1    C   16| PC0                         PD4 / a7 | 1         20 | PD3 / a4
        PC2 / scl |2    H   15| Vdd                   PD5 / a5 / utx | 2    C    19 | PD2 / a3
              PC3 |3    3   14| gnd                   PD6 / a6 / urx | 3    H    18 | PD1 / swio
              PC4 |4    2   13| PA2 / osc0 / a0           nrst / PD7 | 4    3    17 | PC7 / miso
       PC6 / mosi |5    V   12| PA1 / osc1 / a1      PA1 / osc0 / a0 | 5    2    16 | PC6 / mosi
       PC7 / miso |6    0   11| nrst / PD7           PA2 / osc1 / a1 | 6    V    15 | PC5 / sck / scl
       PD1 / swio |7    0   10| PD6 / a6 / urx                   gnd | 7    0    14 | PC4 / a2
         PD4 / a7 |8    3    9| PD5 / a5 / utx                   PD0 | 8    0    13 | PC3
                  +-----------+                                  Vdd | 9    3    12 | PC2 / scl
                                                                 PC0 | 10        11 | PC1 / sda
                                                                     +--------------+
*/


#ifndef in_ch32v003_gpio
  #define in_ch32v003_gpio

  /* ----------------------------------------------------------------------------------------------
     Max. Frequenz fuer GPIO als Ausgang, moegliche Werte fuer GPIO_SPEEDx sind:
       - GPIO_CFGLR_OUT_2Mhz_PP
       - GPIO_CFGLR_OUT_10Mhz_PP
       - GPIO_CFGLR_OUT_50Mhz_PP

     Mode fuer GPIO als Eingaenge, moegliche Werte sind:
       - GPIO_CFGLR_IN_FLOAT        // keine Pullup-Widerstaende
       - GPIO_CFGLR_IN_PUPD         // je nach gesetztem Pin Pullup- oder Pulldownwiderstand

     ---------------------------------------------------------------------------------------------- */

  #define GPIO_SPEEDA             GPIO_CFGLR_OUT_50Mhz_PP
  #define GPIO_SPEEDC             GPIO_CFGLR_OUT_50Mhz_PP
  #define GPIO_SPEEDD             GPIO_CFGLR_OUT_50Mhz_PP

/* -----------------------------------------------
                PORT-A Outputs
   ----------------------------------------------- */

  #define PA1_output_init()       funPinMode(PA1, GPIO_SPEEDA)
  #define PA1_clr()               funDigitalWrite(PA1, 0)
  #define PA1_set()               funDigitalWrite(PA1, 1)

  #define PA2_output_init()       funPinMode(PA2, GPIO_SPEEDA)
  #define PA2_clr()               funDigitalWrite(PA2, 0)
  #define PA2_set()               funDigitalWrite(PA2, 1)

/* -----------------------------------------------
                PORT-A Inputs
   ----------------------------------------------- */

   #define PA1_input_init()       funPinMode(PA1, GPIO_CFGLR_IN_PUPD)
   #define PA1_float_init()       funPinMode(PA1, GPIO_CFGLR_IN_FLOAT)
   #define is_PA1()               funDigitalRead(PA1)

   #define PA2_input_init()       funPinMode(PA2, GPIO_CFGLR_IN_PUPD)
   #define PA2_float_init()       funPinMode(PA2, GPIO_CFGLR_IN_FLOAT)
   #define is_PA2()               funDigitalRead(PA2)

/* -----------------------------------------------
                PORT-C Outputs
   ----------------------------------------------- */

  #define PC0_output_init()       funPinMode(PC0, GPIO_SPEEDC)
  #define PC0_clr()               funDigitalWrite(PC0, 0)
  #define PC0_set()               funDigitalWrite(PC0, 1)

  #define PC1_output_init()       funPinMode(PC1, GPIO_SPEEDC)
  #define PC1_clr()               funDigitalWrite(PC1, 0)
  #define PC1_set()               funDigitalWrite(PC1, 1)

  #define PC2_output_init()       funPinMode(PC2, GPIO_SPEEDC)
  #define PC2_clr()               funDigitalWrite(PC2, 0)
  #define PC2_set()               funDigitalWrite(PC2, 1)

  #define PC3_output_init()       funPinMode(PC3, GPIO_SPEEDC)
  #define PC3_clr()               funDigitalWrite(PC3, 0)
  #define PC3_set()               funDigitalWrite(PC3, 1)

  #define PC4_output_init()       funPinMode(PC4, GPIO_SPEEDC)
  #define PC4_clr()               funDigitalWrite(PC4, 0)
  #define PC4_set()               funDigitalWrite(PC4, 1)

  #define PC5_output_init()       funPinMode(PC5, GPIO_SPEEDC)
  #define PC5_clr()               funDigitalWrite(PC5, 0)
  #define PC5_set()               funDigitalWrite(PC5, 1)

  #define PC6_output_init()       funPinMode(PC6, GPIO_SPEEDC)
  #define PC6_clr()               funDigitalWrite(PC6, 0)
  #define PC6_set()               funDigitalWrite(PC6, 1)

  #define PC7_output_init()       funPinMode(PC7, GPIO_SPEEDC)
  #define PC7_clr()               funDigitalWrite(PC7, 0)
  #define PC7_set()               funDigitalWrite(PC7, 1)

/* -----------------------------------------------
                PORT-C Inputs
   ----------------------------------------------- */

   #define PC0_input_init()       funPinMode(PC0, GPIO_CFGLR_IN_PUPD)
   #define PC0_float_init()       funPinMode(PC0, GPIO_CFGLR_IN_FLOAT)
   #define is_PC0()               funDigitalRead(PC0)

   #define PC1_input_init()       funPinMode(PC1, GPIO_CFGLR_IN_PUPD)
   #define PC1_float_init()       funPinMode(PC1, GPIO_CFGLR_IN_FLOAT)
   #define is_PC1()               funDigitalRead(PC1)

   #define PC2_input_init()       funPinMode(PC2, GPIO_CFGLR_IN_PUPD)
   #define PC2_float_init()       funPinMode(PC2, GPIO_CFGLR_IN_FLOAT)
   #define is_PC2()               funDigitalRead(PC2)

   #define PC3_input_init()       funPinMode(PC3, GPIO_CFGLR_IN_PUPD)
   #define PC3_float_init()       funPinMode(PC3, GPIO_CFGLR_IN_FLOAT)
   #define is_PC3()               funDigitalRead(PC3)

   #define PC4_input_init()       funPinMode(PC4, GPIO_CFGLR_IN_PUPD)
   #define PC4_float_init()       funPinMode(PC4, GPIO_CFGLR_IN_FLOAT)
   #define is_PC4()               funDigitalRead(PC4)

   #define PC5_input_init()       funPinMode(PC5, GPIO_CFGLR_IN_PUPD)
   #define PC5_float_init()       funPinMode(PC5, GPIO_CFGLR_IN_FLOAT)
   #define is_PC5()               funDigitalRead(PC5)

   #define PC6_input_init()       funPinMode(PC6, GPIO_CFGLR_IN_PUPD)
   #define PC6_float_init()       funPinMode(PC6, GPIO_CFGLR_IN_FLOAT)
   #define is_PC6()               funDigitalRead(PC6)

   #define PC7_input_init()       funPinMode(PC7, GPIO_CFGLR_IN_PUPD)
   #define PC7_float_init()       funPinMode(PC7, GPIO_CFGLR_IN_FLOAT)
   #define is_PC7()               funDigitalRead(PC7)

/* -----------------------------------------------
                PORT-D Outputs
   ----------------------------------------------- */

  #define PD0_output_init()       funPinMode(PD0, GPIO_SPEEDD)
  #define PD0_clr()               funDigitalWrite(PD0, 0)
  #define PD0_set()               funDigitalWrite(PD0, 1)

  #define PD1_output_init()       funPinMode(PD1, GPIO_SPEEDD)
  #define PD1_clr()               funDigitalWrite(PD1, 0)
  #define PD1_set()               funDigitalWrite(PD1, 1)

  #define PD2_output_init()       funPinMode(PD2, GPIO_SPEEDD)
  #define PD2_clr()               funDigitalWrite(PD2, 0)
  #define PD2_set()               funDigitalWrite(PD2, 1)

  #define PD3_output_init()       funPinMode(PD3, GPIO_SPEEDD)
  #define PD3_clr()               funDigitalWrite(PD3, 0)
  #define PD3_set()               funDigitalWrite(PD3, 1)

  #define PD4_output_init()       funPinMode(PD4, GPIO_SPEEDD)
  #define PD4_clr()               funDigitalWrite(PD4, 0)
  #define PD4_set()               funDigitalWrite(PD4, 1)

  #define PD5_output_init()       funPinMode(PD5, GPIO_SPEEDD)
  #define PD5_clr()               funDigitalWrite(PD5, 0)
  #define PD5_set()               funDigitalWrite(PD5, 1)

  #define PD6_output_init()       funPinMode(PD6, GPIO_SPEEDD)
  #define PD6_clr()               funDigitalWrite(PD6, 0)
  #define PD6_set()               funDigitalWrite(PD6, 1)

  #define PD7_output_init()       funPinMode(PD7, GPIO_SPEEDD)
  #define PD7_clr()               funDigitalWrite(PD7, 0)
  #define PD7_set()               funDigitalWrite(PD7, 1)

/* -----------------------------------------------
                PORT-D Inputs
   ----------------------------------------------- */

   #define PD0_input_init()       funPinMode(PD0, GPIO_CFGLR_IN_PUPD)
   #define PD0_float_init()       funPinMode(PD0, GPIO_CFGLR_IN_FLOAT)
   #define is_PD0()               funDigitalRead(PD0)

   #define PD1_input_init()       funPinMode(PD1, GPIO_CFGLR_IN_PUPD)
   #define PD1_float_init()       funPinMode(PD1, GPIO_CFGLR_IN_FLOAT)
   #define is_PD1()               funDigitalRead(PD1)

   #define PD2_input_init()       funPinMode(PD2, GPIO_CFGLR_IN_PUPD)
   #define PD2_float_init()       funPinMode(PD2, GPIO_CFGLR_IN_FLOAT)
   #define is_PD2()               funDigitalRead(PD2)

   #define PD3_input_init()       funPinMode(PD3, GPIO_CFGLR_IN_PUPD)
   #define PD3_float_init()       funPinMode(PD3, GPIO_CFGLR_IN_FLOAT)
   #define is_PD3()               funDigitalRead(PD3)

   #define PD4_input_init()       funPinMode(PD4, GPIO_CFGLR_IN_PUPD)
   #define PD4_float_init()       funPinMode(PD4, GPIO_CFGLR_IN_FLOAT)
   #define is_PD4()               funDigitalRead(PD4)

   #define PD5_input_init()       funPinMode(PD5, GPIO_CFGLR_IN_PUPD)
   #define PD5_float_init()       funPinMode(PD5, GPIO_CFGLR_IN_FLOAT)
   #define is_PD5()               funDigitalRead(PD5)

   #define PD6_input_init()       funPinMode(PD6, GPIO_CFGLR_IN_PUPD)
   #define PD6_float_init()       funPinMode(PD6, GPIO_CFGLR_IN_FLOAT)
   #define is_PD6()               funDigitalRead(PD6)

   #define PD7_input_init()       funPinMode(PD7, GPIO_CFGLR_IN_PUPD)
   #define PD7_float_init()       funPinMode(PD7, GPIO_CFGLR_IN_FLOAT)
   #define is_PD7()               funDigitalRead(PD7)

#endif
