# HARDWARE-AUFBAUANLEITUNG & SCHALTPLAN
## 2-Arduino-Mikrocontroller-Teststand (Arduino UNO R4)
### Fachschule Technik Elektrotechnik – Werner-von-Siemens-Schule Hildesheim

---

## 1. Benötigte Hardware-Komponenten

| Anzahl | Komponente | Spezifikation / Bemerkung |
| :--- | :--- | :--- |
| 2× | **Arduino UNO R4** (Minima oder WiFi) | 32-Bit Renesas RA4M1 ARM Cortex-M4, 5V Logikpegel, native `Serial1`-Schnittstelle |
| 1× | **HD44780 LC-Display 20×4** | Inkl. I2C-Backpack (PCF8574, Standard-Adresse `0x27` oder `0x3F`) |
| 1× | **Lineares Dreh-Potentiometer** | $10\,\text{k}\Omega$ (oder $1\,\text{k}\Omega \dots 50\,\text{k}\Omega$) als analoger Signalgeber |
| 1× | **Hardware-Taster (Schließer)** | Für Steckbrett / Panel-Montage (Hold-Trigger) |
| 1× | **Steckbrett (Breadboard)** | Für Laborversuch & fliegende Verdrahtung |
| ca. 15×| **Jumper-Kabel** (Dupont) | Male-Male und Male-Female |
| 2× | **USB-C Kabel** | Zur Programmierung, Diagnose & Spannungsversorgung |

---

## 2. Vollständige Verdrahtungstabelle (Pin-to-Pin)

```
+---------------------------------------------------------------------------------------------------------+
|                                        SCHALTUNGS-TOPOLOGIE                                             |
|                                                                                                         |
|       +-----------------------------------+             +------------------------------------+          |
|       |     SENDER: ARDUINO UNO R4        |             |      EMPFÄNGER: ARDUINO UNO R4     |          |
|       |                                   |             |                                    |          |
|       |  [Pin 5V] ----+                   |             |  [Pin 5V] ----+                    |          |
|       |  [Pin GND] ---+-- (Gemeinsame     |             |  [Pin GND] ---+-- (Gemeinsame      |          |
|       |               |    Masse)         |             |               |    Masse)          |          |
|       |               |        |          |             |               |        |           |          |
|       |  [Pin A0]     |        |          |             |  [Pin A4/SDA] |        |           |          |
|       |  [Pin 1 (TX)] |        |          |             |  [Pin A5/SCL] |        |           |          |
|       +-------|-------|--------|----------+             |  [Pin 4 (D4)] |        |           |          |
|               |       |        |                        |  [Pin 0 (RX)] |        |           |          |
|               |       |        |                        +-------|-------|--------|-----------+          |
|               |       |        |                                |       |        |                      |
|               v       |        +================== GND =========+       |        |                      |
|         +-----------+ |        |                                        |        |                      |
|         | POTI 10k  | |        |          UART-Bus (5V TTL)             |        |                      |
|         | Pin 1: 5V |<+        +--- TX (Pin 1) =======> RX (Pin 0) -----+        |                      |
|         | Pin 2: A0 |                                                            |                      |
|         | Pin 3: GND|<-----------------------------------------------------------+                      |
|         +-----------+                                                            |                      |
|                                                     +----------------------------+                      |
|                                                     |                                                   |
|                                                     v                                                   |
|                                            +------------------+             +-----------------+         |
|                                            |  I2C LCD 20x4    |             |  HOLD-TASTER    |         |
|                                            |  VCC  -> 5V      |             |  Pin 1 -> D4    |         |
|                                            |  GND  -> GND     |             |  Pin 2 -> GND   |         |
|                                            |  SDA  -> A4      |             +-----------------+         |
|                                            |  SCL  -> A5      |                                         |
|                                            +------------------+                                         |
+---------------------------------------------------------------------------------------------------------+
```

### 2.1 Sender-Arduino (Datenerfassung)

| Arduino-Pin | Verbunden mit | Kabelfarbe (Empfehlung) | Funktion / Signal |
| :--- | :--- | :--- | :--- |
| **5V** | Poti Pin 1 (äußerer Kontakt) | Rot | $+5{,}0\,\text{V}$ Versorgungsspannung Poti |
| **GND** | Poti Pin 3 (äußerer Kontakt) | Schwarz | Bezugsmasse $0\,\text{V}$ |
| **A0** | Poti Pin 2 (mittlerer Schleifer) | Gelb | Analoges Eingangssignal ($0 \dots 5\,\text{V}$) |
| **Pin 1 (TX)** | Empfänger-Arduino **Pin 0 (RX)** | Grün | Serieller Bus (UART TX $\rightarrow$ RX) |
| **GND** | Empfänger-Arduino **GND** | Schwarz / Blau | **Zwingender Potenzialausgleich (Gemeinsame Masse)** |

### 2.2 Empfänger-Arduino (Auswertung & HMI)

| Arduino-Pin | Verbunden mit | Kabelfarbe (Empfehlung) | Funktion / Signal |
| :--- | :--- | :--- | :--- |
| **Pin 0 (RX)** | Sender-Arduino **Pin 1 (TX)** | Grün | Serieller Datenempfang |
| **Pin 4 (D4)** | Taster Kontakt 1 | Weiß / Grau | Digitaler Eingang Hold-Trigger (`INPUT_PULLUP`) |
| **GND** | Taster Kontakt 2 | Schwarz | Schaltet Pin 4 auf Masse (Active LOW) |
| **5V** | LCD I2C Backpack **VCC** | Rot | $+5{,}0\,\text{V}$ Display-Stromversorgung |
| **GND** | LCD I2C Backpack **GND** | Schwarz | Display-Masse |
| **A4 (SDA)** | LCD I2C Backpack **SDA** | Blau | I2C Datenleitung |
| **A5 (SCL)** | LCD I2C Backpack **SCL** | Gelb | I2C Taktleitung |

> [!IMPORTANT]
> **Gemeinsame Masse (GND):**
> Die Massepins (`GND`) beider Arduino UNO R4 Boards **müssen direkt miteinander verbunden sein**. Ohne diese Masseverbindung floatingen die Signalpegel der UART-Schnittstelle, was zu Framing-Fehlern und Verbindungsabbrüchen führt!

---

## 3. Besonderheit des Arduino UNO R4

Auf dem klassischen Arduino Uno R3 teilen sich die Pins 0/1 dieselbe serielle Schnittstelle wie der USB-Anschluss.

Beim **Arduino UNO R4** ist dies wesentlich besser gelöst:
* **`Serial` (USB):** Eigener nativer USB-CDC-Kanal zum PC. Wird für den Arduino Serial Monitor und Serial Plotter genutzt.
* **`Serial1` (Pin 0/1):** Vollkommen unabhängiger, nativer Hardware-UART-Kanal für die Kommunikation zwischen den beiden Arduinos.

**Vorteil:** Sie können beide Arduinos gleichzeitig am PC angeschlossen lassen, Code hochladen und im Serial Monitor debuggen, während der Bus auf `Serial1` störungsfrei weiterläuft!

---

## 4. Schritt-für-Schritt Inbetriebnahmeanleitung

### Schritt 1: Benötigte Arduino-Bibliothek installieren
Auf dem Empfänger-Arduino wird die Standard-Bibliothek für I2C-Displays benötigt:
1. Öffnen Sie die **Arduino IDE**.
2. Gehen Sie auf `Werkzeuge` $\rightarrow$ `Bibliotheken verwalten...` (oder `Strg + Umschalt + I`).
3. Suchen Sie nach: **`LiquidCrystal I2C`** (von *Frank de Brabander* oder *Marco Schwartz*).
4. Klicken Sie auf **Installieren**.

### Schritt 2: Sketche hochladen
1. Verbinden Sie den **Sender-Arduino** via USB mit dem PC.
   * Wählen Sie in der IDE: Board **Arduino UNO R4 Minima** (oder WiFi) und den entsprechenden COM-Port.
   * Öffnen Sie den Sketch [`Sender_Datenerfassung/Sender_Datenerfassung.ino`](file:///c:/Users/Janis%20Ole/Downloads/Projekt_Kirchner_Unterricht/Sender_Datenerfassung/Sender_Datenerfassung.ino).
   * Klicken Sie auf **Hochladen**.
2. Verbinden Sie den **Empfänger-Arduino** via USB mit dem PC.
   * Wählen Sie den entsprechenden COM-Port des Empfängers.
   * Öffnen Sie den Sketch [`Empfaenger_Auswertung_HMI/Empfaenger_Auswertung_HMI.ino`](file:///c:/Users/Janis%20Ole/Downloads/Projekt_Kirchner_Unterricht/Empfaenger_Auswertung_HMI/Empfaenger_Auswertung_HMI.ino).
   * Klicken Sie auf **Hochladen**.

### Schritt 3: Display-Kontrast einstellen
Falls nach dem Einschalten nur helle Kästchen oder kein Text auf dem LCD zu sehen sind:
* Drehen Sie mit einem kleinen Schraubendreher am blauen **Potentiometer auf der Rückseite des I2C-Backpacks**, bis die Schrift scharf und kontrastreich erscheint.

---

## 5. Durchführung der Abnahmetests im Labor

1. **Systemstart prüfen (TC-Boot):**
   * Nach dem Einschalten zeigt das Display für 2 Sekunden den Boot-Screen `TESTSTAND V1.2`, danach geht es in `MOD: WAIT TASTE` über.
2. **Hold-Messwertübernahme prüfen (TC-06):**
   * Drehen Sie am Potentiometer. Die Anzeige bleibt statisch auf dem alten Wert stehen (Hold-Modus).
   * Drücken Sie den Taster an Pin D4: Der aktuelle Wert wird sofort übernommen.
   * Zeile 1 zeigt den 10-Bit-Binärwert (z. B. `BIN: 00 1111 1111` bei 255 Digits).
   * Zeile 2 zeigt den Dezimalwert (z. B. `DEC:  255 (Avg:10)`).
3. **Plausibilitäts- und Sprungalarm testen (TC-07):**
   * Stellen Sie das Poti auf ca. $2\,\text{V}$ ($400\,\text{Digits}$) und drücken Sie die Taste $\rightarrow$ `DEV: +00.0% [OK]`.
   * Drehen Sie das Poti schlagartig auf ca. $4{,}5\,\text{V}$ ($900\,\text{Digits}$) und drücken Sie die Taste.
   * Die relative Abweichung beträgt $> 70\,\%$: Zeile 3 schaltet auf `DEV: +125.0% [ALARM]`.
   * Drehen Sie zurück in den Toleranzbereich und drücken Sie erneut: Der Alarm wird zurückgesetzt (`[OK]`).
4. **Verbindungsabbruch / Timeout testen (TC-04):**
   * Ziehen Sie im laufenden Betrieb das grüne TX/RX-Kabel zwischen den Arduinos ab.
   * Nach exakt $2{,}0\,\text{s}$ fängt in Zeile 4 der Text `MOD: ERR! TIMEOUT` an zu blinken. Der letzte Messwert bleibt auf dem Display eingefroren.
   * Stecken Sie das Kabel wieder ein: Das System kehrt sofort automatisch in den Normalbetrieb zurück.
