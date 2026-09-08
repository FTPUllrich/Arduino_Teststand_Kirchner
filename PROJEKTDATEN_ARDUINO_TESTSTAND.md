# 📋 Projektdaten & Spezifikationsübersicht: Mikrocontroller-Teststand (2× Arduino UNO R4)
**Stand:** Version 1.2 (Abgestimmt auf die VDI 2206 / VDI 2221 & Schulcheckliste Herr Kirchner)  
**Verbindliche Referenzdokumente:**
* Ausführliches Lastenheft: [Anforderungsspezifikation_Arduino_Teststand.md](Anforderungsspezifikation_Arduino_Teststand.md)
* Tabellarische Matrix (Excel): [Anforderungsprotokoll_Arduino_Teststand_v1.2.xlsx](Anforderungsprotokoll_Arduino_Teststand_v1.2.xlsx)
* Audit der 11 Kriterien: [Anforderungs_Audit_und_Konsequenzen.md](Anforderungs_Audit_und_Konsequenzen.md)
* Methodisches V-Modell: [V-MODELL_ARDUINO_TESTSTAND.md](V-MODELL_ARDUINO_TESTSTAND.md)
* Gesprächsleitfaden Lehrer: [GESPRAECHSGRUNDLAGE_LEHRERTERMIN.md](GESPRAECHSGRUNDLAGE_LEHRERTERMIN.md)

---

## 1. Systemarchitektur & Schnittstellentopologie (UNO R4)

Das Gesamtsystem besteht aus zwei gekoppelten Arduino UNO R4 (Renesas RA4M1, 32-Bit ARM Cortex-M4, 48 MHz):

```
┌───────────────────────────────────────────────────────────────────────────┐
│ SENDER-ARDUINO (UNO R4 Datenerfassung)                                    │
│ • Analogeingang A0 (0..5,00 V DC, 10-Bit ADC, 0..1023 Digits)             │
│ • Gleitender arithmetischer Mittelwert (N = 10 Samples, dt = 50ms +- 2ms)  │
│ • Zyklische Übertragung (>= 10 Hz / <= 100ms) via Hardware-UART Serial1   │
│ • Paralleles USB-Debugging via Serial zum PC                              │
└─────────────────────────────────────┬─────────────────────────────────────┘
                                      │ TX (Pin 1) - Serial1
                                      │ UART 5V TTL (9600 Baud, 8N1)
                                      │ Zwingend gemeinsame Bezugsmasse (GND)
                                      ▼ RX (Pin 0) - Serial1
┌───────────────────────────────────────────────────────────────────────────┐
│ EMPFÄNGER-ARDUINO (UNO R4 Auswertung & HMI)                               │
│ • Empfang & Telegramm-Parsing: <VAL:xxxx;RAW:xxxx>                        │
│ • Autonome Timeout-Überwachung (> 2,0 s -> LCD: ERR: COMM TIMEOUT!)       │
│ • Grenzwertüberwachung (|Delta| >= 70,0% bzw. >= 50 Digits bei < 10 Dig.) │
│ • Hold-Funktion über Taster an Pin D4 (entprellt t_deb >= 50 ms)          │
│ • I2C-Ansteuerung (A4=SDA, A5=SCL, 100 kHz Busfrequenz)                   │
└─────────────────────────────────────┬─────────────────────────────────────┘
                                      │ I2C (PCF8574 Backpack, Adr. 0x27/0x3F)
                                      ▼
┌───────────────────────────────────────────────────────────────────────────┐
│ DISPLAY (HD44780 LCD 20×4)                                                │
│ Zeile 1: BIN: 00 0110 0100   (10-Bit-Binärwert mit 4er-Nibble-Trennung)   │
│ Zeile 2: DEC:  400 (Avg:10)  (Dezimalwert & Filterangabe)                 │
│ Zeile 3: DEV: +00.0% [OK]    (Sprungabweichung & Plausibilitätsstatus)    │
│ Zeile 4: MOD: HOLD [TASTE]   (Betriebsmodus bzw. ERR: COMM TIMEOUT!)      │
└───────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Eindeutiger & streitsicherer Anforderungskatalog (VDI 2206 / VDI 2221)

Alle Festforderungen (**F**) und Wunschforderungen (**W**) sind gemäß den 11 Kriterien der Schulcheckliste mit quantitativen Toleranzen und nachweisbaren Prüfmethoden definiert:

| ID | Kategorie | Spezifikation (WAS) | Art | Eindeutiges Ziel- & Akzeptanzkriterium (Streitsicher) | Nachweis- / Prüfmethode | Prio |
| :--- | :--- | :--- | :---: | :--- | :--- | :---: |
| **A-01** | Sensorik | Analoge Eingangserfassung | **F** | Quantisierung $0 \dots 1023\,\text{Digits}$ ($0 \dots 5{,}00\,\text{V}$); Linearitätsfehler $\le \pm 2\,\text{LSB}$. | Kalibrierte DC-Spannungsquelle & DMM | Hoch |
| **A-02** | Signalverarbeitung | Rauschfilterung / Mittelwert | **F** | Gleitender Mittelwert über genau $N = 10$ Samples; Abtastintervall $\Delta t = 50\,\text{ms} \pm 2\,\text{ms}$ ($20\,\text{Hz}$). | Signalvergleich Roh vs. Filter im Serial Plotter | Hoch |
| **A-03** | Skalierung | Multi-Kanal-Erweiterung | **W** | Hard- und Softwarearchitektur vorbereitet für bis zu 6 analoge Geber (Pins `A0`–`A5`). | Funktionstest mit Analogsignalgebern | Mittel |
| **A-04** | Kommunikation | Zyklische Busübertragung | **F** | Rate $f_{\text{bus}} \ge 10\,\text{Hz}$ ($\Delta t \le 100\,\text{ms}$); Telegramm `<VAL:xxxx;RAW:xxxx>\n` bei $9600\,\text{Baud}$ 8N1. | Logikanalysator / Trace mit Zeitstempeln | Hoch |
| **A-05** | Systemrobustheit | Bus-Timeout-Überwachung | **F** | Autonome Fehlererkennung bei Telegrammausfall $> 2{,}0\,\text{s}$; Wiederanlaufzeit $\le 200\,\text{ms}$. | Trennung der RX/TX-Signalleitung im Betrieb | Hoch |
| **A-06** | HMI / Display | LCD-Ansteuerung | **F** | 4 Zeilen à 20 Zeichen, synchrone 2-Draht-Ansteuerung via I2C ($100\,\text{kHz}$, PCF8574). | I2C-Bus-Scan / Oszilloskop-Signaltest | Hoch |
| **A-07** | HMI / Format | Binäre Messwertdarstellung | **F** | Exaktes Format `BIN: bb bbbb bbbb` mit führenden Nullen und 4er-Nibble-Trennung ($0 \dots 1023$). | Sichtprüfung über gesamten Stellbereich | Mittel |
| **A-08** | HMI / Bedienung | Hold-Trigger / Taster | **F** | Aktualisierung ausschließlich bei fallender Flanke an Pin D4; Entprellzeit $t_{\text{deb}} \ge 50\,\text{ms}$. | Prellprüfung mit Oszilloskop am Tastereingang | Hoch |
| **A-09** | Systemdynamik | HMI-Reaktionszeit | **F** | Gesamtlatenzzeit zwischen Tasterflanke und Display-Aktualisierung $t_{\text{lat}} \le 100\,\text{ms}$. | 2-Kanal-Oszilloskop (Flanke D4 $\rightarrow$ I2C Paket) | Mittel |
| **A-10** | Systemstart | Init-Verhalten | **F** | Boot-Screen $2{,}0\,\text{s} \pm 0{,}1\,\text{s}$, Ringpuffer-Vorbefüllung $N=10$, Status `MOD: WAIT TASTE`. | Kaltstartprüfung mit Netztrennung | Mittel |
| **A-11** | Logik & Alarm | Plausibilitätsüberwachung | **F** | Optischer Alarm `[ALARM!]` bei relativem Sprung $|\Delta_{\text{rel}}| \ge 70{,}0\,\%$; Absolutschwelle $|\Delta| \ge 50\,\text{Digits}$ bei Werten $< 10\,\text{Digits}$. | Sprungantwort-Test am Potentiometer | Hoch |
| **A-12** | Logik & Alarm | Alarm-Rücksetzung | **F** | Alarm bleibt aktiv, bis ein Messwert im Toleranzbereich ($< 70\,\%$) per Taster bestätigt wird. | Stufentest mit Signalrückkehr | Hoch |
| **A-13** | Datenspeicher | Status-Vorhaltung | **F** | Verzögerungsfreie Bereitstellung von aktuellem Wert, Vorgängerwert und Status im SRAM. | Code-Review & Verifikation im Debugger | Hoch |
| **A-14** | Elektrik & Schutz | Potenzialausgleich & Pegel | **F** | Versorgungsspannung $+5{,}0\,\text{V} \pm 5\,\%$; zwingende GND-Kopplung ($0{,}0\,\text{V}$ Differenz); Ruheschutz durch `INPUT_PULLUP`. | DMM-Spannungsmessung an allen Knoten | Hoch |
| **A-15** | Umwelt | Betriebsumgebung | **F** | Betriebsbereich $+10\,^\circ\text{C} \dots +40\,^\circ\text{C}$, relative Luftfeuchte $20\,\% \dots 80\,\%$ (nicht kondensierend), IP20. | Sicht- und Funktionsprüfung unter Laborbedingungen | Niedrig |

*Legende: F = Festforderung (Muss), W = Wunschforderung (Soll/Kann)*
