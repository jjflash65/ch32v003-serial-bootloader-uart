![CH32V003 UART-Bootloader](https://github.com/jjflash65/ch32v003_uart_bootloader/blob/master/picture/ch32v003_nano_bootloader.gif)
<br><br>
<table border="0" width="99%">
  <tbody>
    <tr>
      <td bgcolor="#999999"><font color="#000000" face="Arial" style="font-size: 15px;">

Auf dieser Eingangsseite jetzt nur in Kürze fürr was dieses Repository 'gut' ist:
<br><br>
Der im Auslieferungszustand enthaltene serielle Bootloader ist für den Bastler nicht so richtig
Alltagstauglch. Auch wenn der Autor nicht wirklich ein Fan von dem Arduino-Framework ist, hat
er dennoch an der Idee des Bootloaders - wenn die Debug-Funktion nicht wirklich gebraucht wird -
gefallen gefunden. Es ist einfach eine schöne, schnelle und 'saubere Sache' einfach nur einen
USB-Stecker in eine Buchse zu stecken und schon kann man loslegen (zumindest was die Hardware
angeht).
<br><br>
Wenn der Autor es korrekt verstanden hat, dann ist der im Auslieferungszustand enthaltene
serielle Bootloader nicht so wirklich brauchbar und ein Flasherprogramm hat er auch nur für
Windows gesehen.
<br><br> auf den Github-Seiten von Christian Lohr gibt es 2 wesentliche Seiten:
<ul>
  <li><a href="https://github.com/cnlohr/ch32fun">https://github.com/cnlohr/ch32fun<br></a><li>
  <li><a href="https://github.com/cnlohr/rv003usb">https://github.com/cnlohr/rv003usb<br></a></li>
<ul>
Ersterer stellt ein grundsätzliches Framework zur Verfügung auf der auch dieser Bootloader hier
aufbaut.
<br>
Die zweite Seite beschäftigt sich mit V-USB und sehr faszinierend hier ist, dass dort auch
ein Bootloader für eben V-USB zur Verfügung gestellt wird. Dieser Bootloader hat gegenüber
diesem UART-Bootloader folgende Vorteile:
<ul>
  <li>deutlich schnellerer Upload</li>
  <li>weniger benötigte Bauteile (2 Zenerdioden und 3 Widerstände, ggf. 3.3V Spannungsregler)
</ul>
Nachteilig jedoch ist, dass mit diesem Bootloader keine serielle Verbindung aus der Firmware
heraus aufgebaut werden kann. Zudem nimmt es von den eh schon knappen GPIO's 2 weitere weg,
wenn es einer seriellen Schnittstelle bedarf. Zudem bedarf es dann einer weiteren USB-Buchse,
die eine Schaltung dann haben muß. Der V-USB Bootloader ist also immer dann von
Vorteil, wenn es keiner seriellen Schnittstelle bedarf. Ein weiterer Nachteil dieses V-USB
Bootloaders ist, dass vor jedem Upload eine Reset-Taste gedrückt werden muß damit der Bootloader
für eine einstellbare Zeit im Bootloader verbleibt um evtl. einen Upload-Befehl entgegen zu
nehmen.
<br><br>
Die Nachteile des V-USB Bootloaders sind dann auch die Vorteile des UART-Bootloaders:
<br><br>
<ul>
  <li>nur eine USB-Buchse notwendig</li>
  <li>vor einem Upload bspw. mittels eines 'make flash' wird durch das Hostprogramm der Controller
      selbsttätig in den Bootmodus gesetzt</li>
  <li>immer noch sehr deutlich schneller als 'ardulink'    
</ul>
<br><br>
Der Bootloader kann entweder zur Not (weil kein Programmer verfügbar ist) mit einem Arduino 
oder einem Programmer geflasht werden. Hierfür sind die Einstellungen im wohl selbsterklärenden 
Makefile entsprechend zu setzen.
<br><br>
Um dieses bewerkstelligen zu können, muß aus einem Arduino ein Programmer gemacht werden und hierfür
ist dann in diesem Repository auch die Firmware für einen Arduino / AVR gegeben.
<br><br>
Das dieser Text hier noch keine Dokumentation ist, ist dann auch das noch große (und sehr ungeliebte)
ToDo für diese Seite.
<br><br>
Für denjenigen, der das dennoch aufbaut:
<br>br>
Viel Spaß damit!
<br><br>
PS: auch wenn der Autor wie beschrieben kein Fan von Arduino ist, hat er dennoch Patches ausgearbeitet,
mit denen es möglich ist, einen CH32V003 auch komfortabel und wie von einem UNO oder Nano gewohnt, den
Chip zu flashen. Hier ist auch die Doku noch das große ToDo... in der Hoffnung auf ein "coming soon"
      </font></td>
    </tr>
  </tbody>
</table>      
