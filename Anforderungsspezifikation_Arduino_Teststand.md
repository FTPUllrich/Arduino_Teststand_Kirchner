# SYSTEM-ANFORDERUNGSSPEZIFIKATION (LASTENHEFT)
## Gemäß VDI 2206 / VDI 2221 & V-Modell (Systemebene / Anwendersicht)

---

### Dokumenten-Metadaten

| Attribut | Wert / Spezifikation |
| :--- | :--- |
| **Projektbezeichnung** | Mikrocontroller-Teststand zur analogen Messwerterfassung, UART-Busübertragung & LCD-Binäranzeige |
| **Projektart** | Betriebliche Projektarbeit / Teststandsentwicklung (2. Schuljahr Technikerschule) |
| **Ausbildungsstätte** | Werner-von-Siemens-Schule Hildesheim – Fachschule Technik (Fachrichtung Elektrotechnik) |
| **Ersteller / Team** | Projektteam Fachschule Technik |
| **Datum / Stand** | 25.08.2026 / Version 1.2 (Finaler Anforderungsstand) |
| **Status / Freigabe** | Vorläufiges Anforderungsdokument zur Abnahme / Diskussionsgrundlage |
| **Referenzdokumente** | `Anforderungsprotokoll_Arduino_Teststand_v1.1.xlsx`, Kickoff-Protokoll & Anforderungsanalyse |

---

## 1. Ausgangslage, Zielsetzung & Situationsbeschreibung

### 1.1 Situationsbeschreibung
Ein Kunde benötigt zur Überwachung analoger Sensoren ein verteiltes, modulares System, bestehend aus zwei Mikrocontrollern (Arduino Uno R3) in einer Master-Slave-ähnlichen Topologie:
1. **Datenerfassungs-Arduino (Sender / Slave):** Erfasst kontinuierlich analoge Sensorsignale über einen integrierten Analog-Digital-Wandler (10-Bit ADC), glättet die Messwerte in Echtzeit und stellt diese zyklisch über einen Kommunikationsbus bereit.
2. **Auswertungs- und HMI-Arduino (Empfänger / Master):** Empfängt die berechneten Messwerte über die serielle Schnittstelle. Über einen Hardware-Taster fordert der Benutzer die Anzeige des aktuellen Messwerts an. Die Darstellung erfolgt auf einem LC-Display im 10-Bit-Binärformat (mit strukturierter Nibble-Trennung) sowie im Dezimalformat. Gleichzeitig überwacht das System sprunghafte Messwertänderungen ($\ge 70\,\%$) als Plausibilitätskontrolle und signalisiert diese optisch.

### 1.2 Zielsetzung des Projekts
Ziel ist die Entwicklung, Dokumentation und Verifikation eines robusten, erweiterbaren Labor-Teststands. Das System muss unempfindlich gegenüber Busunterbrechungen sein, definierte Einschaltzustände besitzen und nach den Qualitätskriterien für mechatronische Spezifikationen (eindeutige Referenzgrößen, überprüfbare Akzeptanzkriterien, vollständige Fehlerbehandlung) aufgebaut werden.

```
+---------------------------------------------------------------------------------------------------------+
|                                              GESAMTSYSTEM                                               |
|                                                                                                         |
|   +-----------------------+                         UART-Bus                         +---------------+  |
|   |  Datenerfassungs-     |  TX (Pin 1 / D2)  ====================>  RX (Pin 0 / D3) | Auswertungs-  |  |
|   |  Arduino (Sender)     |                                                          | Arduino (HMI) |  |
|   +-----------------------+  GND (0V)         --------------------   GND (0V)        +---------------+  |
|          ^         ^                                                                     |       |      |
|          |         |                                                             I2C Bus |       |      |
|   +----------+ +-------+                                                             (A4/A5) |       |      |
|   | Analog-  | | +5V / |                                                                 v       v      |
|   | sensor   | | GND   |                                                            +---------+ +-----+ |
|   | (Poti)   | +-------+                                                            | LCD     | |Taste| |
|   +----------+                                                                      | 20x4    | |(D4) | |
|                                                                                     +---------+ +-----+ |
+---------------------------------------------------------------------------------------------------------+
```

---

## 2. Systemarchitektur, Grenzen & Schnittstellen

### 2.1 Teilsystem 1: Datenerfassungs-Arduino (Sender)
* **Funktion:** Kontinuierliche Signalabtastung, Rauschunterdrückung durch gleitenden Mittelwert, zyklische Telegrammübertragung via UART.
* **Hardware:** Arduino Uno R3 (ATmega328P, 16 MHz, 5 V Logikpegel).
* **Eingänge:** Analogeingang `A0` (erweiterbar auf `A1`–`A5`), Messbereich $0{,}0\,\text{V} \dots 5{,}0\,\text{V}$ DC ($0 \dots 1023\,\text{Digits}$).
* **Ausgänge:** UART-TX (Hardware-UART `Pin 1` oder SoftwareSerial `Pin D2`), Baudrate: $9600\,\text{Baud}$ (konfigurierbar bis $115200\,\text{Baud}$).

### 2.2 Teilsystem 2: Auswertungs- & HMI-Arduino (Empfänger)
* **Funktion:** Serieller Datenempfang, Plausibilitätsprüfung (Sprungüberwachung $\ge 70\,\%$), Tasterentprellung, Hold-Messwertspeicherung, Ansteuerung des LCD 20×4 via I2C, Verbindungsüberwachung (Timeout $> 2\,\text{s}$).
* **Hardware:** Arduino Uno R3 (ATmega328P, 16 MHz, 5 V Logikpegel).
* **Eingänge:**
  * UART-RX (`Pin 0` bzw. `Pin D3`), Empfang der gefilterten Messwerte.
  * Digitaler Taster-Eingang `Pin D4` (mit internem Pull-Up-Widerstand, active LOW) zur Messwertübernahme (Hold-Trigger).
* **Ausgänge:** I2C-Schnittstelle (`SDA` an `Pin A4`, `SCL` an `Pin A5`, 100 kHz Standard Mode) zur Ansteuerung des LCD-Backpacks (PCF8574, I2C-Adresse `0x27` oder `0x3F`).

### 2.3 Gesamtsystem & Bezugspotenzial
* **Spannungsversorgung:** $+5{,}0\,\text{V}\,\text{DC} \pm 5\,\%$ via USB-Schnittstelle oder geregeltes Labornetzteil.
* **Gemeinsames Bezugspotenzial:** Zwingende galvanische Verbindung der Masseleitungen (`GND` zu `GND`) zwischen Sender, Empfänger, Sensor und Display zur Gewährleistung stabiler UART- und I2C-Signalpegel.

### 2.4 Schnittstellen- und Verdrahtungsübersicht

| Baugruppe | Komponente | Arduino-Pin | Signal / Funktion | Elektrischer Pegel | Spezifikation / Bemerkung |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Sender-Arduino** | Analogsensor (Poti) | `Pin A0` | Analog IN (Messgröße) | $0 \dots 5\,\text{V}$ DC | 10-Bit ADC ($0 \dots 1023\,\text{Digits}$), Ri $\approx 10\,\text{k}\Omega$ |
| **Sender-Arduino** | UART TX | `Pin 1` / `D2` | Serielle Datenübertragung | 5 V TTL (zyklisch $\ge 10\,\text{Hz}$) | Verbindung zu RX des Empfänger-Arduinos |
| **Sender-Arduino** | Spannungsversorgung | `5V` / `GND` | Betriebsspannung | $+5\,\text{V}$ DC / $0\,\text{V}$ | Gemeinsame Masse mit Gesamtsystem |
| **Empfänger-Arduino** | UART RX | `Pin 0` / `D3` | Serieller Datenempfang | 5 V TTL | Empfang des gleitenden Mittelwerts |
| **Empfänger-Arduino** | I2C LCD Backpack | `Pin A4` (SDA) | I2C Datenleitung | 5 V TTL (Pull-Up) | Standardadresse `0x27` oder `0x3F` |
| **Empfänger-Arduino** | I2C LCD Backpack | `Pin A5` (SCL) | I2C Taktleitung | 5 V TTL (Pull-Up) | Taktfrequenz $100\,\text{kHz}$ |
| **Empfänger-Arduino** | Aktualisierungs-Taster | `Pin D4` (od. `D2`) | Digital IN (Hold-Trigger) | Active LOW (`INPUT_PULLUP`) | Entprellzeit $t_{\text{deb}} \ge 50\,\text{ms}$ |
| **Gesamtsystem** | Potenzialausgleich | `GND - GND` | Gemeinsame Bezugsmasse | $0\,\text{V}$ | Zwingend für Signalintegrität |

---

## 3. Vollständiger Anforderungskatalog (Lastenheft nach VDI 2206 / VDI 2221)

> **Klassifizierung (Art):**
> * **F = Festforderung (Muss-Kriterium):** Zwingend für die Abnahme erforderlich.
> * **W = Wunschforderung (Soll-/Kann-Kriterium):** Vorgesehen für Skalierungsphase 2.

| ID | Kategorie | Anforderung / Spezifikation | Art | Zielwert / Akzeptanzkriterium | Verifikationsmethode (Abnahmetest) | Prio |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **A-01** | Sensorik & Erfassung | **Analoge Eingangserfassung** | **F** | Erfassung eines kontinuierlichen Analogsignals über 10-Bit-ADC ($0 \dots 1023\,\text{Digits} \hat{=} 0{,}00 \dots 5{,}00\,\text{V}$) an Analogeingang `A0`. Linearitätsfehler $\le \pm 2\,\text{LSB}$. | Kalibrierte DC-Spannungsquelle & Digitalmultimeter (DMM) | Hoch |
| **A-02** | Signalverarbeitung | **Rauschunterdrückung & Glättung** | **F** | Berechnung des gleitenden arithmetischen Mittelwerts über exakt $N = 10$ aufeinanderfolgende Samples. Abtastintervall $\Delta t = 50\,\text{ms} \pm 2\,\text{ms}$ (Abtastrate $20\,\text{Hz}$). | Signalvergleich Roh- vs. Filterwert im Arduino Serial Plotter bei verrauschtem Signal | Hoch |
| **A-03** | Systemskalierung | **Erweiterbarkeit der Eingangskanäle** | **W** | Hard- und Softwarearchitektur ausgelegt für modulare Erweiterung auf bis zu 6 Analogkanäle (`A0`–`A5`). | Funktionstest mit bis zu 6 analogen Signalgebern | Mittel |
| **A-04** | Kommunikation | **Zyklische Bus-Datenübertragung** | **F** | Zyklische Übertragung des gemittelten Werts via UART-Bus mit einer Übertragungsrate $f_{\text{bus}} \ge 10\,\text{Hz}$ (Intervall $\le 100\,\text{ms}$). Baudrate $9600\,\text{Baud}$, 8N1, mit Framing `<VAL:xxxx;RAW:xxxx\n>`. | Logikanalysator / Serial-Monitor-Trace mit Zeitstempeln | Hoch |
| **A-05** | Systemrobustheit | **Bus-Timeout & Verbindungsüberwachung** | **F** | Automatische Erkennung eines Signal- oder Telegrammausfalls. Bei Ausbleiben gültiger Daten für $t_{\text{timeout}} > 2{,}0\,\text{s}$ muss eine Fehlermeldung auf dem LCD ausgegeben werden. | Unterbrechung der RX/TX-Signalleitung im laufenden Betrieb | Hoch |
| **A-06** | HMI / Display | **LC-Display Ansteuerung via I2C** | **F** | Anbindung eines HD44780-kompatiblen LC-Displays (20 Zeichen × 4 Zeilen) über I2C-Backpack (PCF8574). I2C-Busfrequenz $100\,\text{kHz}$. | I2C-Bus-Scan / Hardwaretest mit Oszilloskop | Hoch |
| **A-07** | HMI / Format | **Binäre Messwertdarstellung** | **F** | Formatierte 10-Bit-Binärausgabe mit führenden Nullen und 4-Bit-Nibble-Strukturierung (Format: `BIN: bb bbbb bbbb`). | Sichtprüfung der LCD-Anzeige über den gesamten Stellbereich ($0 \dots 1023$) | Mittel |
| **A-08** | HMI / Bedienung | **Messwert-Aktualisierung (Hold-Trigger)** | **F** | Aktualisierung des Anzeigewerts ausschließlich bei Betätigung des Tasters (Hold-Funktion bis zum nächsten Druck). Software-Entprellzeit $t_{\text{deb}} \ge 50\,\text{ms}$. | Prellprüfung / Funktionstest mit Oszilloskop am Tastereingang | Hoch |
| **A-09** | Systemdynamik | **Reaktionszeit HMI-Aktualisierung** | **F** | Latenzzeit zwischen Tasterbetätigung (fallende Flanke) und vollständiger Aktualisierung des LCD-Inhalts $t_{\text{lat}} \le 100\,\text{ms}$. | 2-Kanal-Oszilloskop (Trigger Flanke Taster $\rightarrow$ erstes I2C-Paket zum Display) | Mittel |
| **A-10** | Systemstart | **Definiertes Initialisierungsverhalten** | **F** | Nach Kaltstart (Power-On): Ausgabe Boot-Screen (2 s), Vorbefüllung des $N=10$ Ringpuffers, Einnahme eines definierten Wartezustands (`MOD: INIT / WAIT TASTE`). | Einschaltprüfung / Kaltstart-Zyklus mit Netztrennung | Mittel |
| **A-11** | Logik & Alarm | **Plausibilitäts- & Grenzwertüberwachung** | **F** | Optischer Alarm auf dem LCD (`DEV: +xx.x% [ALARM]`), wenn die relative Änderung $\|Wert_k - Wert_{k-1}\| \ge 0{,}70 \cdot Wert_{k-1}$ beträgt. Bei $Wert_{k-1} < 10\,\text{Digits}$ greift eine Mindest-Absolutschwelle von $\|\Delta\| \ge 50\,\text{Digits}$. | Sprungantwort-Test mit Rechteck-Signalgenerator / Spannungssprung am Poti | Hoch |
| **A-12** | Logik & Alarm | **Alarm-Rücksetzung / Quittierung** | **F** | Der optische Alarmzustand bleibt aktiv, bis ein neuer Messwert innerhalb des Toleranzbereichs ($< 70\,\%$) per Tastendruck übernommen wird. | Stufentest mit Rückkehr in den Nennbereich | Hoch |
| **A-13** | Datenspeicher | **Direkter Datenzugriff im Speicher** | **F** | Strukturierte Vorhaltung von aktuellem Messwert ($Wert_k$), Vorgängerwert ($Wert_{k-1}$), Filterpuffer und Systemstatus-Flags im flüchtigen SRAM. | Code-Review & RAM-Belegungsanalyse via Compiler / Debug-Ausgabe | Hoch |
| **A-14** | Elektrik & Schutz | **Betriebsspannung & Schutzbeschaltung** | **F** | Versorgungsspannung $+5{,}0\,\text{V}\,\text{DC} \pm 5\,\%$. Schutz vor Kurzschluss und undefinierten Pegeln durch interne Pull-Up-Widerstände an allen Digitaleingängen. | DMM-Spannungsmessung & Verifikationsprüfung offener Pins | Hoch |
| **A-15** | Umgebungsbed. | **Betriebsumgebung & Laborbetrieb** | **F** | Zulässiger Betriebstemperaturbereich $+10\,^\circ\text{C} \dots +40\,^\circ\text{C}$, relative Luftfeuchtigkeit $20\,\% \dots 80\,\%$ (nicht kondensierend), Schutzart IP20 (Laboraufbau). | Sicht- und Funktionsprüfung unter Laborbedingungen | Niedrig |

---

## 4. Spezifikation der Algorithmen & mathematischen Logik

### 4.1 Signalfilterung (Gleitender Mittelwert)
Zur Eliminierung hochfrequenter Einstreuungen und Potentiometer-Rauschens wird ein gleitender Mittelwert (Moving Average) über einen FIFO-Ringpuffer der Länge $N = 10$ berechnet:

$$\bar{x}_k = \frac{1}{N} \sum_{i=0}^{N-1} x_{k-i} = \frac{1}{10} \sum_{i=0}^{9} x_{k-i}$$

* **Abtasttakt:** Nicht-blockierend über `millis()` alle $\Delta t = 50\,\text{ms}$ ($20\,\text{Hz}$).
* **Puffer-Initialisierung (Startup):** Beim Systemstart wird der Puffer in der Setup-Routine mit 10 aufeinanderfolgenden ADC-Messungen gefüllt, um Einschwingverzögerungen und Fehlmessungen beim ersten Tastendruck zu verhindern.

### 4.2 Plausibilitäts- und Sprungüberwachung ($\ge 70\,\%$-Schwelle)
Um fehlerhafte Sensorsprünge, Leitungsunterbrechungen oder extreme Prozessabweichungen zu detektieren, wird bei jedem Hold-Tastendruck die Abweichung zum Vorgängerwert $Wert_{k-1}$ berechnet:

$$\Delta_{\text{rel}} = \frac{|Wert_k - Wert_{k-1}|}{Wert_{k-1}} \cdot 100\,\%$$

#### Robuste Division-by-Zero- und Rauschabsicherung:
Liegt der Vorgängerwert im Nullpunkts- oder Rauschbereich ($Wert_{k-1} < 10\text{ Digits}$), ist eine prozentuale Berechnung mathematisch instabil bzw. führt zu Division durch Null. Daher gilt folgende zweistufige Auswertelogik:

* **Fall 1: Normalbetrieb ($Wert_{k-1} \ge 10\text{ Digits}$):**
  $$\text{ALARM} = \text{TRUE} \quad \Longleftrightarrow \quad \frac{|Wert_k - Wert_{k-1}|}{Wert_{k-1}} \ge 0{,}70$$
* **Fall 2: Nullpunkts- / Rauschbereich ($Wert_{k-1} < 10\text{ Digits}$):**
  $$\text{ALARM} = \text{TRUE} \quad \Longleftrightarrow \quad |Wert_k - Wert_{k-1}| \ge 50\text{ Digits}$$
* **Standardzustand (Kein Sprung / Toleranzbereich eingehalten):**
  $$\text{ALARM} = \text{FALSE}$$

* Beim allerersten Tastendruck nach dem Systemstart ($k=1$) wird $Wert_1$ als neuer Referenzwert geladen, ohne einen Alarm auszulösen.

---

## 5. HMI- & Display-Spezifikation (LCD 20×4 I2C)

### 5.1 Zeilenstruktur und Layout
Das LC-Display wird über die I2C-Adresse `0x27` (bzw. `0x3F`) angesteuert. Es ist in vier feste Informationszeilen unterteilt:

```
+--------------------+
|BIN: 00 1111 1111   |  <-- Zeile 1: 10-Bit Binärwert (mit 4er-Nibble-Trennung)
|DEC: 1023 (Avg:10)  |  <-- Zeile 2: Dezimalwert (0..1023) & Filterangabe
|DEV: +00.0% [OK]    |  <-- Zeile 3: Prozentuale Abweichung & Status
|MOD: HOLD [TASTE]   |  <-- Zeile 4: Systemstatus / Bedienhinweis
+--------------------+
```

### 5.2 Feldspezifikation der Display-Zeilen

| Zeile | Format-Template | Bedeutung & Dynamisches Verhalten |
| :--- | :--- | :--- |
| **Zeile 1** | `BIN: bb bbbb bbbb` | **10-Bit-Binärdarstellung:** Führende Nullen werden immer angezeigt. Trennung in Bit 9..8, Bit 7..4 und Bit 3..0 zur optimalen Lesbarkeit. |
| **Zeile 2** | `DEC: xxxx (Avg:10)` | **Dezimalwert:** $0 \dots 1023$ Digits, rechtsbündig formatiert mit Leerzeichen. Angabe der Filtertiefe $N=10$. |
| **Zeile 3** | `DEV: ±xx.x% [OK]` <br> `DEV: +xx.x% [ALARM]` | **Abweichung & Plausibilität:** Vorzeichenbehaftete prozentuale Differenz zum Vorgängerwert. Bei Sprung $\ge 70\,\%$ Wechsel auf `[ALARM]`. |
| **Zeile 4** | `MOD: HOLD [TASTE]` <br> `MOD: ERR! TIMEOUT` | **Betriebsmodus:** Zeigt `HOLD [TASTE]` im Normalbetrieb. Bei Ausfall der UART-Verbindung ($> 2\,\text{s}$) blinkende Anzeige `ERR! TIMEOUT`. |

---

## 6. Kommunikationsprotokoll (UART-Bus)

* **Physikalische Schicht:** Asynchrone serielle Schnittstelle (5 V TTL), Baudrate $9600\,\text{Baud}$, 8 Datenbits, keine Parität (N), 1 Stoppbit (8N1).
* **Übertragungszyklus:** Zyklisch alle $80\,\text{ms} \dots 100\,\text{ms}$ ($f \ge 10\,\text{Hz}$).
* **Telegrammformat (ASCII-Frame):**
  ```
  <VAL:xxxx;RAW:xxxx>\n
  ```
  * `<` : Start-Delimiter
  * `VAL:xxxx` : Gleitender Mittelwert (4-stellig, $0000 \dots 1023$)
  * `;` : Feldtrenner
  * `RAW:xxxx` : Ungefilterter Rohwert des AD-Wandlers
  * `>` : End-Delimiter
  * `\n` : Zeilenumbruch (Linefeed `0x0A`)
* **Vorteil:** Robuste Rahmenerkennung, unempfindlich gegenüber einzelnen Bitfehlern, direkt im Arduino Serial Monitor les- und debugbar.

---

## 7. Fehlerbehandlung & Ausnahmeszenarien (Robustheit)

```
                              +--------------------+
                              | Normalbetrieb      |
                              | Datenempfang OK    |
                              +--------------------+
                                        |
                             UART-Ausfall > 2,0 s
                                        |
                                        v
                              +--------------------+
                              | TIMEOUT-ZUSTAND    |
                              | LCD Zeile 4 blinkt |
                              | Messwert Freeze    |
                              +--------------------+
                                        |
                             Gültiges Telegramm
                             wieder empfangen
                                        |
                                        v
                              +--------------------+
                              | Automatische       |
                              | Wiederaufnahme     |
                              +--------------------+
```

1. **Bus-Unterbrechung / Telegrammausfall ($t > 2{,}0\,\text{s}$):**
   * Der Empfänger startet bei jedem gültigen Telegramm einen Software-Watchdog-Timer.
   * Läuft der Timer ab, wechselt Zeile 4 auf `MOD: ERR! TIMEOUT` (blinkend im 500 ms Takt).
   * Der zuletzt gültige Messwert bleibt eingefroren auf dem Display stehen (kein Crash, kein Reset).
   * Sobald wieder gültige UART-Frames eintreffen, wird der Fehlerstatus automatisch zurückgesetzt (**Auto-Recovery**).
2. **Sensorabriss / Kurzschluss ($0\,\text{V}$ oder $5\,\text{V}$ Dauersignal):**
   * ADC liefert $0\,\text{Digits}$ bzw. $1023\,\text{Digits}$. Der Wert wird normgerecht als $0\,\text{V}$ bzw. $5\,\text{V}$ verarbeitet.
3. **Spannungsausfall / Kaltstart:**
   * Nach Wiederkehr der Betriebsspannung durchläuft das System die definierte Initialisierungsroutine (A-10) ohne undefinierte Pin-Zustände.

---

## 8. Parametermatrix & Konfigurationsgrenzen (Modularer Parametersatz)

Um eine flexible Anpassung an unterschiedliche Sensoren, Busgeschwindigkeiten und Laboranforderungen zu gewährleisten, sind alle Kernparameter im Quellcode modular über Konfigurationskonstanten parametrierbar definiert:

| Parameter-Bezeichnung | Code-Konstante | Standardwert (Default) | Zulässiger Einstellbereich | Einheit / Typ | Beschreibung |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Filtertiefe (Mittelwert)** | `FILTER_WINDOW_SIZE` | `10` | $1 \dots 50$ | Samples (`uint8_t`) | Anzahl der Samples im Gleitenden Mittelwert-Ringpuffer |
| **Abtastintervall ADC** | `SAMPLE_INTERVAL_MS` | `50` | $10 \dots 500$ | Millisekunden (`uint16_t`) | Zykluszeit der analogen Messwerterfassung ($20\,\text{Hz}$) |
| **UART-Senderate** | `BUS_TX_INTERVAL_MS` | `100` | $20 \dots 1000$ | Millisekunden (`uint16_t`) | Zykluszeit der seriellen Busübertragung ($\ge 10\,\text{Hz}$) |
| **UART-Baudrate** | `UART_BAUDRATE` | `9600` | $9600 \dots 115200$ | Baud (`uint32_t`) | Übertragungsgeschwindigkeit der seriellen Schnittstelle |
| **Bus-Timeout-Schwelle** | `BUS_TIMEOUT_MS` | `2000` | $500 \dots 10000$ | Millisekunden (`uint16_t`) | Maximale Wartezeit bis zur `TIMEOUT`-Fehlermeldung |
| **Sprung-Alarmschwelle** | `ALARM_DEV_THRESHOLD_PCT`| `70.0` | $10.0 \dots 100.0$ | Prozent (`float`) | Prozentuale relative Abweichung für Plausibilitätsalarm |
| **Mindest-Sprungschwelle**| `ALARM_MIN_ABS_DIFF` | `50` | $10 \dots 200$ | Digits (`uint16_t`) | Mindest-Absolutwertdifferenz bei Werten nahe $0\,\text{V}$ |
| **Taster-Entprellzeit** | `DEBOUNCE_TIME_MS` | `50` | $20 \dots 200$ | Millisekunden (`uint16_t`) | Software-Entprellzeit für den Hold-Taster |
| **I2C-Display-Adresse** | `LCD_I2C_ADDR` | `0x27` | `0x27` od. `0x3F` | Hex (`uint8_t`) | I2C-Adresse des PCF8574 LCD-Backpacks |

---

## 9. Skalierungs- & Zukunftskonzept (Phase 2 - Erweiterbarkeit)

* **Hardwareseitig:** Vorhaltung der Pins `A1` bis `A5` auf dem Sender-Arduino für Mehrkanal-Betrieb (z. B. Spannungs- und Strommessung).
* **Protokollseitig:** Erweiterung des Telegramms auf `<CH:x;VAL:xxxx;RAW:xxxx>\n`.
* **Bedienkonzept:** Umschaltung des aktiven Anzeigekanals auf dem Empfänger durch langen Tastendruck ($t > 2\,\text{s}$) oder zusätzlichen Hardware-Taster.

---

## 10. Verifikations- und Testmatrix (V-Modell Systemebene / Abnahmetests)

| Test-ID | Zugehörige Anforderung | Prüfaufbau & Testschritte | Erwartetes Ergebnis (Pass-Kriterium) | Status |
| :--- | :--- | :--- | :--- | :--- |
| **TC-01** | A-01 (ADC-Erfassung) | Kalibrierte Gleichspannung in 0,5-V-Schritten von $0{,}0\,\text{V}$ bis $5{,}0\,\text{V}$ an `A0` anlegen. Vergleich mit DMM. | ADC-Werte entsprechen der Kennlinie $Digits = \frac{U_{\text{in}}}{5{,}0\,\text{V}} \cdot 1023 \pm 2\,\text{Digits}$. | Bereit |
| **TC-02** | A-02 (Glättung) | Verrauschtes Signal anlegen. Ausgabe von Raw- und Filterwert im Serial Plotter vergleichen. | Rauschamplitude im Filterwert um mindestens Faktor 3 gedämpft; Phasenverzug $\le 250\,\text{ms}$. | Bereit |
| **TC-03** | A-04 (Bus-Zyklus) | Logikanalysator an TX/RX anschließen und 100 aufeinanderfolgende Telegramme aufzeichnen. | Mittleres Übertragungsintervall $\le 100\,\text{ms}$ ($f \ge 10\,\text{Hz}$), fehlerfreies Frame-Parsing. | Bereit |
| **TC-04** | A-05 / TC-Timeout | Im laufenden Betrieb die RX-Leitung am Empfänger abziehen. | Nach $2{,}0\,\text{s} \pm 0{,}2\,\text{s}$ erscheint `ERR! TIMEOUT` in Zeile 4. Nach Wiederverbinden automatische Rückkehr. | Bereit |
| **TC-05** | A-07 (Binärformat) | Stellwerte $0$, $1$, $255$, $512$, $1023$ einstellen und Displayausgabe prüfen. | Exakte Nibble-Ausgabe: z. B. `BIN: 00 1111 1111` für $255$ bzw. `BIN: 11 1111 1111` für $1023$. | Bereit |
| **TC-06** | A-08 / A-09 (Taster) | Taster betätigen; 2-Kanal-Oszilloskop an Tasterpin und I2C-SCL anschließen. | Displaywert bleibt bis zum Tastendruck statisch (Hold). Reaktionszeit vom Tastendruck bis I2C-Start $\le 100\,\text{ms}$. | Bereit |
| **TC-07** | A-11 / A-12 (70%-Alarm)| Spannungssprung von $2{,}0\,\text{V}$ ($409\,\text{Digits}$) auf $4{,}0\,\text{V}$ ($819\,\text{Digits}$) anlegen und Taste drücken. | Zeile 3 zeigt `DEV: +100.0% [ALARM]`. Nach Rückkehr auf $2{,}1\,\text{V}$ und erneutem Druck schaltet Status auf `[OK]`. | Bereit |

---

## 11. Freigabe & Abnahme

| Funktion | Name / Rolle | Datum | Unterschrift |
| :--- | :--- | :--- | :--- |
| **Ersteller (Projektteam)** | Fachschüler Elektrotechnik | 25.08.2026 | *gezeichnet Projektteam* |
| **Prüfer / Fachebene** | Projektteam Fachschule Technik | 25.08.2026 | *gezeichnet QM* |
| **Freigabe Kunde / Dozent** | Dozent / Projektleiter Werner-von-Siemens-Schule | | |
