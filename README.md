# Air-quality-sensor

## 1 Voraussetzung

Ein Server auf dem eine Homeassistant Instanz l"auft und eine Mqtt Integration installiert und eingerichtet wurde.

## 2 Software

### Flashen mit VSCode und der PlatformIO Erweiterung

1. Clone das Github Repository.
2.  ̈Offne das Repository als Projekt.
3.  ̈Andere die server, user und pass Variablen auf die eigenen Werte.
4. Verbinde den PC per USB mit dem Mikrocontroller.
5. Uploade das Projekt auf den Mikrocontroller.
6. Der Mikrocontroller  ̈offnet dann einen Accsess Point mit dem man
sich mit einem Wlan f ̈ahigen Ger ̈at verbinden kann.
7. Der Accsess point leitet einem zu einer Webseite weiter, auf der
man sein WLAN Netzwerk und WLAN Passwort eingeben kann
(die Eingabe wird auch  ̈uber Neustarts gespeichert).
8. Danach werden automatisch alle 5 Minuten neue Werte an den Ho-
meassistant gesendet und dort angezeigt.

### Flaschen mit der Arduino IDE

1. Downloade die main.cpp Datei aus dem src Ordner des Github Re-
positorys.
2. Installiere die Bibliotheken WiFiManager, Seeed HM330X, Ardui-
noJson und PubSubClient.
3. weiter bei Punkt drei von 2.1

## Hardware

### Stromversorgung

1. Die vier Akkus m ̈ussen in paralleler Schaltung zusammen gel ̈otet
werden und dann mit dem Batterie Stecker des Solarmoduls ver-
bunden werden.
2. Die Akkus und das Solarmodul k ̈onnen in dem 3D-druckbaren Ge-
h ̈ause untergebracht werden(power top.stl und power bottom.stl).
3. Auf der Unterseite des Solarmoduls muss die Spannung der Solar-
zelle eingestellt werden.
4. Die Kabel des Solarzelle m ̈ussen an den Solarstecker des Solamoduls
angeschlossen werden.
5. Danach muss der Boot Knopf des Solarmoduls gedr ̈uckt werden um
das Modul zu starten.
6. Dann ist der USB-Stromausgang aktiv

### Sensor

Der Sensor und der Mikrocontroller k ̈onnen beide auf dem 3D-druckbaren
Halter befestigt werden (sensormount.stl).
Wenn die Software auf dem Mikrocontroller geflasht wurde und die Kabel
wie auf dem Diagramm zu sehen, mit den Sensor verbunden wurden, kann
der Mikrocontroller per USB mit Strom versorgt werden.


### Gesamtsystem

Danach kann alles auf der Unterseite der Solarzelle befestigt werden und
das Paneel in der N ̈ahe von einer Straße und WLAN platziert werden.
Das System sollte so platziert werden, dass der Sensor m ̈oglichst guten
Luftzugang hat, aber die Unterseite auch gut vor Wasser gesch ̈utzt ist.
Wenn das System l ̈auft sind Automatisch drei Sensoren im HomeAssi-
stant verf ̈ugbar
