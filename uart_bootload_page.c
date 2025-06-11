/* --------------------------------------------------------
                        uart_bootload.c

     Serieller Bootloader fuer CH32V003 Mikrocontroller

     Anmerkungen zum Flashspeicher:

     Es ist keine einfache Schreibmoeglichkeit gegeben, ein
     ueberschreiben eines Bytes ist nicht moeglich, es muss
     zuvor geloescht sein.

     Da der Flashspeicher langsamer ist als der Core
     Daten anfragt, muss ab bei einem Coretakt > 25 MHz
     eine Latenzzeit eingelegt werden (CH32FUN laueft
     standardmaessig mit 48 MHz Coretakt, deshalb hier
     eine Latenz einfuegen).

     Vorgehen:
        - Flashspeicher unlock
        - Flashspeicher loeschen
        - Werte von Host lesen und Flash schreiben
        - Flashspeicher lock

     TxD an PD5, RxD an PD6

     MCU : ch32v003

     08.06.2025  R. Seelig
   -------------------------------------------------------- */

/*
     Proprietaeres Uebertragungsprotokoll:

          Host sendet        CH32V003 antwortet
       ----------------------------------------------
             "u"                 "o"
       ----------------------------------------------
             Anzahl
             64 Byte
             Bloecke
        (2 Byte binaer)
       ----------------------------------------------
                             Echo der Anzahl der
                             Bloecke
       ----------------------------------------------
           n-Anzahl
       64 Byte Bloecke
                             nach jedem empfangenem
                             Block wird ein "o"
                             gesendet
       ----------------------------------------------
                             Start des
                             Userprogramms
       ----------------------------------------------
*/

#include <stdint.h>

#include "ch32fun.h"
#include "ch32v003_gpio.h"

#include "uart.h"

#define baud             57600                 // bei schnelleren Baudraten kann sich der Boot-
                                               // loader verschlucken, wenn eine Firmware mit
                                               // Uart-Ausgabe am Laufen war

#define timeout          1800                  // Zeit in ms die auf ein Flashkommando gewartet
                                               // wird bis das User-Programm startet 

#define flash_base_addr  0x08000000ul          // hier beginnt der Flashspeicherbereich
                                             

#define CR_PAGE_FTPG    ((uint32_t)0x00010000) // warum auch immer: In CH32FUN nicht deklariert 
                                             

#define stat_led_enable  1                     // 1 : eine LED wird beim Flashvorgang eingeschaltet
                                               // 0 : Status-LED Funktion abgeschaltet              
                                             
// bei eingeschalteter LED-Statusanzeige hier den Portpin
// angeben an den die LED angeschlossen sein soll
#define led_init()   PD4_output_init()
#define led_set()    PD4_set()
#define led_clr()    PD4_clr()

#define crlf()       { uart_putchar('\n'); uart_putchar('\r'); }

#define testfuncs    0                      // 1 : Zu Testzwecken werden innerhalb des Bootloaders
                                            //     zur Laufzeit Ausgaben gemacht, bspw. einen Hexdump

uint8_t  pagebuffer[68];

/* ------------------------------------------------------------
                    Speicherzugriff-Makros
   ------------------------------------------------------------ */
#define fmem_readbyte(addr)  *(uint8_t*)(addr)     // Byte aus Speicher lesen
#define fmem_readword(addr)  *(uint16_t*)(addr)    // Word aus Speicher lesen

#define flash_setlatency()   ( FLASH->ACTLR = FLASH_Latency_1 )   // bei Coretakt < 24MHz FLASH_Latency_0

/* ------------------------------------------------------------
                         hexnibbleout

     gibt die unteren 4 Bits eines chars als Hexaziffer aus.
     Eine Pruefung ob die oberen vier Bits geloescht sind
     erfolgt NICHT !

     Uebergabe:
           b  : enthaelt auf den unteren 4 Bits des Bytes
                das auszugebende Nibble
  -------------------------------------------------------------  */
void hexnibbleout(uint8_t b)
{
  if (b< 10) b+= '0'; else b+= 55;
  uart_putchar(b);
}

/* ------------------------------------------------------------
                              hex8_out

     gibt einen 8-Bit Wert (Byte) als Hexaziffer aus.

     Uebergabe:
           b  : auszugebendes Byte
  -------------------------------------------------------------  */
void hex8_out(uint8_t b)
{
  hexnibbleout(b >> 4);
  hexnibbleout(b & 0x0f);
}

#if (testfuncs == 1)
  /* ------------------------------------------------------------
                               hex16_out

       gibt einen 16-Bit Wert (Word) als Hexaziffer aus.

       Uebergabe:
             w  : auszugebendes Word
    -------------------------------------------------------------  */
  void hex16_out(uint16_t w)
  {
    hex8_out(w >> 8);
    hex8_out(w & 0x0ff);
  }

  /* ------------------------------------------------------------
       hex32_out

       gibt einen 32-Bit Wert als Hexaziffer aus.

       Uebergabe:
             d  : auszugebender Wert
    -------------------------------------------------------------  */

  void hex32_out(uint32_t d)
  {
    hex16_out(d >> 16);
    hex16_out(d & 0x0000ffff);
  }

  uint16_t hex16_read(void)
  {
    uint8_t  i;
    uint8_t  c;
    uint16_t w;

    w= 0;
    for (i= 0; i< 4; i++)
    {
      w= w << 4;
      c= uart_getchar();
      if (c >= 'a')
      {
        c= c-'a' + 10;
      }
      else
      if (c >= 'A')
      {
        c= c-'A' + 10;
      }
      else
      {
        c -= '0';
      }
      w |= c;
    }
    return w;
  }

  /* ----------------------------------------------------------
     uart_puts

     gibt einen Text an dem UART aus

        c : Zeiger auf den AsciiZ - STring
     ----------------------------------------------------------*/
  void uart_puts(char *c)
  {
    while (*c)
    {
      uart_putchar(*c++);
    }
  }

#endif

/* ----------------------------------------------------------
     inline Funktionen als Abkuerzung fuer Schreibfaule :-)
   ---------------------------------------------------------- */

static inline uint8_t flash_is_busy()
{
  return ((FLASH->STATR & FLASH_STATR_BSY) == FLASH_STATR_BSY);
}

static inline void flash_wait_until_not_busy()
{
  while(flash_is_busy()) {}
}

/* ----------------------------------------------------------
                        flash_unlock

     entsperrt den Flashspeicher zum Beschreiben oder
     Loeschen
   ---------------------------------------------------------- */
static inline void flash_unlock()
{
  // KEY1 zum Entsperren zuerst
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
	FLASH->MODEKEYR = FLASH_KEY1;
	FLASH->MODEKEYR = FLASH_KEY2;  
}

/* ----------------------------------------------------------
                         flash_lock

     sperrt den Flashspeicher, Beschreiben oder
     Loeschen aus dem Programm heraus nicht mehr moeglich
   ---------------------------------------------------------- */
static inline void flash_lock()
{
  FLASH->CTLR |= FLASH_CTLR_LOCK;
}

/* ----------------------------------------------------------
                         fmem_writeword

     Beschreibt eine Speicherstelle im Flashspeicher mit
     einem 16-Bit Wert.

     Uebergabe:
        addr:    bezeichnet die Adresse innerhalb einer
                 Page und NICHT die Speicheradresse absolut
                 gesehen
        data:    zu schreibender 16-Bit Wert
   ---------------------------------------------------------- */
static inline void fmem_writeword(uint32_t addr, uint16_t data)
{
  if(FLASH->CTLR & FLASH_CTLR_LOCK) { return; }   // wenn Flash gelockt Funktion verlassen

  flash_wait_until_not_busy();

  FLASH->CTLR |= CR_PG_Set;                       // beschreiben Flash zulassen

  *(uint16_t*)(uintptr_t)addr = data;             // Datenwert schreiben

  flash_wait_until_not_busy();

  FLASH->CTLR &= CR_PG_Reset;                     // beschreiben Flash sperren
}

/* ----------------------------------------------------------
                        flash_erase_sector

     loescht einen Speichersektor (1 kByte) ab der angegebenen
     Speicheraddresse 'addr'
   ---------------------------------------------------------- */
static inline void flash_erase_sector(uint32_t addr)
{
  // bei gelocktem Flash Funktion verlassen
  if(FLASH->CTLR & FLASH_CTLR_LOCK) { return; }

  flash_wait_until_not_busy();

  FLASH->CTLR |= CR_PER_Set;              // sector erase bit im Control Register

  FLASH->ADDR = addr;                     // zu loeschende Speicheradresse

  FLASH->CTLR |= CR_STRT_Set;             // Loeschvorgang starten

  flash_wait_until_not_busy();            // warten bis Loeschvorgang beendet

  FLASH->CTLR &= CR_PER_Reset;            // Reset sector erase bit
}

/* ----------------------------------------------------------
                        flash_erase

     loescht den gesamten Flash-Speicher
   ---------------------------------------------------------- */
static inline void flash_erase(void)
{
  // bei gelocktem Flash Funktion verlassen
  if(FLASH->CTLR & FLASH_CTLR_LOCK) { return; }

  flash_wait_until_not_busy();

  FLASH->CTLR |= CR_MER_Set;              // sector erase bit im Control Register

  FLASH->CTLR |= CR_STRT_Set;             // Loeschvorgang starten

  flash_wait_until_not_busy();            // warten bis Loeschvorgang beendet

  FLASH->CTLR &= CR_MER_Reset;            // Reset sector erase bit
}

/* ----------------------------------------------------------
                        start_user_prog

     verlaesst den Bootloader und startet das Anwederprogramm.
     Die Funktion ist nach einem Timeout aufzurufen, wenn
     nach PowerOn kein Kommando zum Neuschreiben des
     Flashspeichers eingegangen ist. Alle Programmanweisungen
     die nacht "start_user_prog" angegeben sind, werden
     logischerweise nicht ausgefuehrt weil dieses hier in
     etwa einem "return to userprogram" oder "jump to
     userprogram" entspricht
   ---------------------------------------------------------- */
static inline void start_user_prog(void)
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;

                          // Reset-Value Status-Register  == 0
                          // Bit14 == Mode
                          //           0 == "after software reset, you
  FLASH->STATR = 0;       //                 can switch to the user-area"

  FLASH->CTLR = CR_LOCK_Set;
  PFIC->SCTLR = 1<<31;
}


/* ----------------------------------------------------------
                         progstart_wait

     wartet die in dtime * ms angegebene Zeit auf das
     Kommando fuer einen Flash-Programmiervorgang

     Uebergabe:
        dtime  : Wartezeit bis zu einem Timeout

     Rueckgabe:
        1 : Kommando ist eingegangen
        0 : Kommando ist nicht eingegangen
   ---------------------------------------------------------- */
uint8_t progstart_wait(uint16_t dtime)
{
  uint16_t cx;

  cx= 0;
  while(cx < dtime)
  {
    if (uart_ischar())
    {
      if (uart_getchar()== 'u') { return 1; }
    }
    cx++;
    delay(1);
  }
  return 0;
}

void fmem_writepage(uint32_t memofs)
{
  uint32_t i, ind, w;
  uint32_t *memptr; 
  
	FLASH->CTLR = CR_BUF_RST | CR_PAGE_FTPG;               // buffer reset und fast programming  
	FLASH->ADDR = memofs + flash_base_addr;
	while( FLASH->STATR & FLASH_STATR_BSY );

  memptr = (uint32_t*)flash_base_addr + (memofs >> 2);   // memofs beinhaltet Bytespeicheroffset und
                                                         // keine 32-Bit Speicheradresse

  // 64-Byte Pagebuffer laden
	for( i = 0; i < 16; i++ )
	{
    ind= i*4;
    w= pagebuffer[ind+3];
    w= (w << 8) + pagebuffer[ind+2];
    w= (w << 8) + pagebuffer[ind+1];
    w= (w << 8) + pagebuffer[ind];
    memptr[i]= w;                                        // Speicher schreiben (noch "temporaer")
		FLASH->CTLR = CR_PAGE_FTPG | FLASH_CTLR_BUF_LOAD;    // Schreibpuffer laden, fast programming setzen
		while( FLASH->STATR & FLASH_STATR_BSY );             // wird nur benoetigt wenn Programm aus RAM
                                                         // laeuft
	}

	FLASH->CTLR = CR_PAGE_PG|CR_STRT_Set;                  // Pagebuffer schreiben (hier wird "geflasht")

	while( FLASH->STATR & FLASH_STATR_BSY );               // warten bis Schreibvorgang fertig
  
}


/* ----------------------------------------------------------
                              main
   ---------------------------------------------------------- */
int main(void)
{
  int      i;
  uint16_t pageanz;
  uint16_t w;
  uint8_t  pgcx;
  uint32_t fofsaddr;

  SystemInit();

  uart_init(baud);
  
  #if (stat_led_enable == 1)
    led_init();
    led_clr();
  #endif  

  fofsaddr= 0x0000;

  if (progstart_wait(timeout))
  {
    flash_setlatency();                              // Waitstates fuer Flashspeicher
    flash_unlock();
    delay(10);
    if(FLASH->CTLR & FLASH_CTLR_FLOCK)               // wenn Fastprogramming nicht gestartet
    {
      start_user_prog();                             // dann zum Userprogramm verzweigen
    }   

    flash_erase();
    delay(50);

    // loescht den Flashspeicher Sektorenweise, bei Bedarf fuer
    // Versuchszwecke zu "entkommentieren"
/*
    for (i= 0; i< 16; i++)
    {
      flash_erase_sector(0x08000000+(i*1024));
      delay(10);
    }
*/

    // Antwort an den Host, dass Bootloader zur Uebernahme bereit ist
    uart_putchar('o');
    
    #if (stat_led_enable == 1)
      led_set();
    #endif
    
    // die Anzahl der zu schreibenden Pages lesen
    w= uart_getchar();
    pageanz= w << 8;
    w= (uart_getchar()) & 0x00ff;
    pageanz |= w;

    // und Anzahl Pages zurueck schicken
    uart_putchar(pageanz >> 8);
    uart_putchar(pageanz & 0x00ff);
    uart_putchar(pageanz >> 8);

//    FLASH->CTLR |= CR_PG_Set;                       // Flash schreiben Standard

    // Bytes einer Pages vom Host einlesen
    for (i=0; i< pageanz; i++)
    {
      for (pgcx= 0; pgcx< 64; pgcx++)
      {
        pagebuffer[pgcx]= uart_getchar();
      }
      
      fmem_writepage(fofsaddr);                     // und Page flashen
      fofsaddr += 64; 
           
      uart_putchar('o');
    }
    FLASH->CTLR &= CR_PG_Reset;                     // beschreiben Flash sperren        
    
    flash_lock();
  }
  crlf();

  #if (stat_led_enable == 1)
    led_set();
  #endif


  #if (testfuncs == 1)
  // zu Debug- und Vergleichszwecken

    uint32_t t_addr;
    uint8_t b;

    t_addr= flash_base_addr + 0x08000000;
        
    crlf(); crlf();
    f_addr= fofsaddr + flash_base_addr;
    hex32_out(f_addr);
    crlf();
    for (w= 0; w< 35; w++)
    {
      t_addr= f_addr;
      hex16_out(f_addr-fofsaddr); uart_putchar(' ');
      for (i= 0; i< 16; i++)
      {
        b= fmem_readbyte(f_addr);
        hex8_out(b);
        uart_putchar(' ');
        f_addr++;
      }
      f_addr= t_addr;
      uart_puts("   ");
      for (i= 0; i< 16; i++)
      {
        b= fmem_readbyte(f_addr);
        if ((b>= ' ' ) && (b <= 127)) { uart_putchar(b); } else { uart_putchar('.'); }
        f_addr++;
      }
      crlf();
    }
    crlf(); crlf();
  #endif

  start_user_prog();

  // hierher sollte der Bootloader nie kommen
  while(1);
}

