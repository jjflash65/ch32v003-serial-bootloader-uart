{* ------------------------------------------------------------------------
                          v003flash.pas

     Hostprogramm zum Upload von Intel-Hex Dateien in einen CH32V003
     Microcontroller.

     Betriebssystem  : Linux
     Compiler        : FreePascal 2.6.4 oder hoeher

     Proprietaeres Uebertragungsprotokoll:

          Host sendet       CH32V003 Bootloader antwortet
       ----------------------------------------------
             "u"                      "o"
       ----------------------------------------------
            32-Bit
         Resetvektor
         Userprogramm
         Binaerformat
       ----------------------------------------------
         Anzahl 64 Byte 
         Bloecke
         (2 Byte binaer)
       ----------------------------------------------
                            Echo der Anzahl der 
                            Bloecke
       ----------------------------------------------
           n-Anzahl
           Byte Bloecke
                            nach jedem empfangenen
                            Block wird ein "o"
                            gesendet
       ----------------------------------------------
                            Start des Userprogramms
       ----------------------------------------------

     06.06.2025  R. Seelig
   ------------------------------------------------------------------------ *}
program ch32flash;

uses
    classes, sysutils, process, synaser;


const
  dtime_clear           = 10;
  dtime_hi              = 25;
  dtime_lo              = 5;

var
  ser         : tblockserial;
  showtxbar   : boolean;
  filename    : string;
  portname    : string;
  devicename  : string;
  binarray    : array[0..65535] of byte;

  pagesize    : byte;                       // nimmt die Anzahl der Byte per Page

  maxadress   : word;


procedure txpbarscala(pbaranzahl : byte);
var
  b, b2, b3       : byte;

begin
  write(' Writing |');
  b2:= pbaranzahl;
  for b:= 0 to b2 do
  begin
    b3:= b mod 10;
    if (b3= 0) then
    begin
      write('|');
    end else
    begin
      write('-');
    end;
  end;
end;

procedure txpbar(startzeit : comp; maxwert : word; value : word; ch : char; pbaranzahl : byte);
var
  r          : real;
  zanz, b    : byte;
  ts         : ttimestamp;
  lz2        : comp;
  prozent    : word;
  newstep    : word;
  xpos       : word;

begin
  newstep:= maxwert div pbaranzahl;
  if newstep= 0 then newstep:= 1;
  if (((value mod newstep)= 0) and (value> 0)) then
  begin
    write(chr(13));
    write(chr(27),'[',10,'C');
    r:= pbaranzahl;
    r:= (r / maxwert);
    zanz:= trunc(r * value);
    for b:= 0 to zanz do write(ch);

    xpos:= pbaranzahl + 1 + 10;
    write(chr(13));
    write(chr(27),'[',xpos,'C');

    prozent:= (value*100) div maxwert;
    ts:= DateTimeToTimeStamp(Now);
    lz2:= TimeStampToMSecs(ts);
    write (' ',prozent,'% ',((lz2 - startzeit) / 1000):3:2,'s  ');
  end;
end;

procedure txpbar100(startzeit : comp; pbaranzahl : byte);
var
  ts              : ttimestamp;
  lz2             : comp;
  prozent         : word;
  xpos            : word;
begin
  xpos:= pbaranzahl + 1 + 9;
  write(chr(13));
  write(chr(27),'[',xpos,'C');

  prozent:= 100;
  ts:= DateTimeToTimeStamp(Now);
  lz2:= TimeStampToMSecs(ts);
  write (' ',prozent,'% ',((lz2 - startzeit) / 1000):3:2,'s  ');
end;


function word2hex(val16 : word) : string;
var
  ub  : byte;
  w   : word;
  hs  : string[6];
begin
  w:= val16;
  ub:= (w shr 12);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= chr(ub);

  w:= val16;
  ub:= (w shr 8);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  w:= val16;
  ub:= (w shr 4);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  w:= val16;
  ub:= w and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  word2hex:= hs;

end;

function byte2hex(val8 : word) : string;
var
  ub  : byte;
  b   : word;
  hs  : string[4];
begin

  b:= val8;
  ub:= (b shr 4);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= chr(ub);

  b:= val8;
  ub:= b and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  byte2hex:= hs;

end;


procedure clr_inpuffer;
var
  ch     : char;
begin
  while (ser.waitingdataex> 0) do
  begin
    ch:= chr(ser.recvbyte(1));
    sleep(dtime_clear);
  end;
end;

procedure wait_inpuffer;
var
  count  : word;
begin
  count:= 0;
  while (ser.waitingdataex= 0) do
  begin
    inc(count);
    if (count > 2000) then
    begin
      ser.free;
      writeln(stderr,'Error: bootloader doesn'#39't respond');      
      halt(1);
    end;
    sleep(1);
  end;
end;

procedure rtsdtr_pulse;
begin
  ser.dtr:= true;
  ser.flush;
  sleep(50);
  ser.dtr:= false;
  ser.flush;
  sleep(50);
  ser.dtr:= true;
  ser.flush;

  ser.rts:= true;
  ser.flush;
  sleep(50);
  ser.rts:= false;
  ser.flush;
  sleep(50);
  ser.rts:= true;
  ser.flush;
end;


function DoExecute(progname, arguments : string) : tstringlist;
var
  myprocess       : tprocess;
  mystringlist    : classes.tstringlist;

begin
  myprocess:= tprocess.create(nil);
  mystringlist:= classes.tstringlist.create;
  myprocess.executable:= progname;
  myprocess.parameters.add(arguments);

  myprocess.options:= myprocess.options + [powaitonexit,pousepipes];
  
  // -----------------------------------------------------------------------
  myprocess.execute;
  // -----------------------------------------------------------------------
  
  exitcode:= myprocess.exitstatus;
  mystringlist.loadfromstream(myprocess.output);

  myprocess.free;  
  DoExecute:= mystringlist;
end;


procedure FlashMCU(datnam : string);
var
  ub          : byte;
  ch          : char;
  cx          : word;
  tdat        : text;
  f           : file;
  s, hs, hs2  : string;
  err         : integer;
  bytelen     : byte;
  fadress     : word;
  highadress  : word;

  banz        : word;
  wcx         : word;
  lz, lz2     : comp;
  ts          : ttimestamp;
  b, b1       : word;
  dumpdebug   : boolean;
  blkanz      : word;

begin
  dumpdebug:= false;
  highadress:= 0;
  
  assign(f,portname);
  {$i-} reset(f); {$i+}
  if (ioresult <> 0) then
  begin
    writeln(stderr,'Error: communication port ',portname,' not found');
    halt(3);
  end else
  begin
    close(f);
  end;
    
  assign(tdat,datnam);
  {$i-} reset(tdat); {$i+}
  if (ioresult <> 0) then
  begin
    writeln(stderr,'Error: file ',datnam,' not found');
    halt(4);
  end;
  
  for cx:= 0 to maxadress do
  begin
    binarray[cx]:= $ff;
  end;

  ts:= DateTimeToTimeStamp(Now);
  lz:= TimeStampToMSecs(ts);


  while(not(eof(tdat))) do
  begin
    readln(tdat,s);

    if (copy(s,1,1)= ':') then                      // Einleitung einer Zeile ":"
    begin
      if (copy(s,8,2)= '00') then                   // Position fuer "normale" Datenbytes
      begin
        hs:= '$'+copy(s,2,2);                       // Position fuer die Anzahl der Datenbytes
        val(hs,bytelen,err);
        dec(bytelen);
        hs:= '$'+copy(s,4,2);                       // Position Hi-Byte Speicheradresse
        val(hs,fadress,err);
        fadress:= fadress * 256;
        hs:= '$'+copy(s,6,2);
        val(hs,ub,err);
        fadress:= fadress+ub;

        for banz:= 0 to (( length(s)-11 ) div 2) - 1 do

        begin
          hs:= copy(s,10 +(banz * 2),1);
          hs2:= '$' + hs + copy(s,11 +(banz * 2),1);
          val(hs2,ub,err);
          binarray[fadress]:= ub;
          inc(fadress);
          if (fadress> highadress) then highadress:= fadress;

        end;

      end;
    end;
  end;
  close(tdat);
  dec(highadress);

  if (highadress > maxadress) then
  begin
    ser.free;
    writeln;
    writeln(stderr,' ERROR 1: Programm stored in hex-file is too large');
    writeln;
    halt(5);
  end;

  blkanz:= highadress div pagesize;
  if ((highadress mod pagesize)<> 0) then inc(blkanz);
  
  // zu Testzwecken, ob die Intel-Hex Datei korrekt eingelesen wird
  if (dumpdebug) then
  begin
    showtxbar:= false;
    writeln('Hexdump'+chr(13)+chr(10));
    writeln(highadress,'   ',blkanz,'   ',pagesize,'   ');

    for wcx:= 0 to highadress do
    begin
      if ((wcx mod 16)= 0) then
      begin
        writeln();
        write(word2hex(wcx),' ');
      end;
      write(byte2hex(binarray[wcx]),' ');
    end;
    writeln();
    writeln(^j,blkanz,^j);
  end;

  rtsdtr_pulse;

  clr_inpuffer;

  sleep(300);
  ser.sendbyte(ord('u'));               // Anfrage ob Bootloader ready
  ser.flush;
  sleep(dtime_lo);

  wait_inpuffer;
  ch:= chr(ser.recvbyte(1));            // Programmer sendet ein 'o' zurueck
  ser.flush;
  sleep(dtime_lo);
  
  clr_inpuffer();
// Anzahl zu schreibender Pagebyte Bloecke senden
  ser.sendbyte(hi(blkanz));
  ser.flush;
  sleep(dtime_lo);

  ser.sendbyte(lo(blkanz));
  ser.flush;
  sleep(dtime_lo);
  
  wait_inpuffer();
  cx:= ser.recvbyte(1);
  banz:= cx * 256;
  wait_inpuffer();
  cx:= ser.recvbyte(1);
  banz:= banz+cx;
  sleep(100);
  
  writeln(' v003flash v0.1 by R. Seelig 2025', chr(10));
  writeln(' Device             : ', devicename);
  writeln(' available flashsize: ', maxadress+1, ' Bytes');
  if (blkanz <> banz) then
  begin
    writeln(^j'     ERROR 2: wrong number of received pages');
    writeln(  '              expected: ',blkanz, ' got: ',banz); 
    exit;
  end;
  writeln(chr(10),' Pages to write: ',banz);
  writeln(        ' Bytes to write: ', (highadress));
  if (showtxbar) then
  begin
    writeln;
    txpbarscala(51);
    write(chr(13));
  end else
    writeln(' please wait...');

  wcx:= 0;
  for b:= 1 to blkanz do
  begin
    clr_inpuffer;  
    for b1:= 1 to pagesize do
    begin

      if (dumpdebug) then
      begin            
        if ((wcx mod 16) = 0) then
        begin
          write(^j,word2hex(wcx),' ');
        end;
        write(byte2hex(binarray[wcx]),' ');
      end;  
    
      ser.sendbyte(binarray[wcx]);
      ser.flush;
      sleep(1);
      inc(wcx);

      if (showtxbar) then
      begin
        txpbar(lz,highadress, wcx, '#', 50);
        write(chr(13));
      end;

    end;
    wait_inpuffer;
    ub:= ser.recvbyte(1);
  end;

  if (showtxbar) then
  begin
    txpbar100(lz, 50);
    writeln;
  end;

  writeln;

  ts:= DateTimeToTimeStamp(Now);
  lz2:= TimeStampToMSecs(ts);
  writeln(' Flashing time: ',((lz2 - lz) / 1000):3:2,'s  ');
  writeln;
  writeln(' v003flash done, thank you');

  sleep(dtime_clear);
//  rtsdtr_pulse;
end;

var
  tstring      : classes.tstringlist;

{*  -----------------------------------------------------------------------
                                M-A-I-N
    ----------------------------------------------------------------------- *}
begin
  pagesize:= 64;                         
  
  if (paramcount < 2) then
  begin
    writeln('v003flash v0.1');
    writeln('by R. Seelig 2025');
    writeln('');
    writeln('Syntax:');
    writeln('v003flash port filename [notxbar]'); writeln();
    writeln('  port      : port were the adapter is connected');
    writeln('  filename  : file to be uploaded');
    writeln('  notxbar   : optional, no bar graph is shown');
    writeln('              (use it by call from an IDE)');
    writeln();
    writeln('  Example   : v003flash /dev/ttyUSB0 helloworld.hex');
    writeln();

    halt(1);
  end;

  portname:= paramstr(1);
  filename:= paramstr(2);
  
  tstring:= classes.tstringlist.create;
  tstring:= DoExecute('fuser',portname);
 
  if (tstring.count > 0) then
  begin

    writeln(stderr,^j''+portname+' is in use with process: ',tstring.strings[tstring.count-1],^j); 

    halt(2);
  end;

  devicename:= 'CH32V003';
  
  maxadress:= $3fff;

  showtxbar:= true;
  if (paramcount > 2) then
  begin
    if (paramstr(3)= 'notxbar') then showtxbar:= false;
  end;

  ser:= tblockserial.create;
  ser.raiseExcept:= false;
  ser.linuxlock:= false;
  ser.connect(portname);
  ser.config(57600, 8, 'N', 1, false, false);

  clr_inpuffer;

  flashmcu(filename);

  ser.free;
  halt(0);
end.
