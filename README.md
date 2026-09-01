# 🚀 2× Arduino UNO R4 Mikrocontroller-Teststand
### Analoge Messwerterfassung, UART-Busübertragung, Plausibilitätsüberwachung & LCD-Binäranzeige
**Fachschule Technik (Elektrotechnik) – Werner-von-Siemens-Schule Hildesheim**  
*Systemtechnik & Anforderungsmanagement nach VDI 2206 / VDI 2221 (Systemebene)*

---

## 🌟 Key Features auf einen Blick

| Feature | Spezifikation / Umsetzung | Nutzen / Funktion |
| :--- | :--- | :--- |
| **🎛️ 10-Bit ADC & Glättung** | $0 \dots 1023\,\text{Digits}$ ($0 \dots 5\,\text{V}$) an Pin `A0`, Moving Average über FIFO-Puffer ($N=10$, $20\,\text{Hz}$) | Beseitigt Signalrauschen & Potentiometer-Flattern in Echtzeit |
| **📡 Dedizierter Hardware-Bus** | Native `Serial1`-Schnittstelle (Pin 0/1, $9600\,\text{Baud}$, 8N1), Frame `<VAL:xxxx;RAW:xxxx>\n` | Keine Störung des USB-Debuggings am PC (paralleler Serial Plotter aktiv) |
| **📟 20×4 I2C LC-Display** | HD44780 mit PCF8574 Backpack an Pin `A4` (SDA) / `A5` (SCL) | Dedizierte Zeilen für Binär-Nibbles, Dezimalwert, Abweichung & Status |
| **🔢 Binärformat mit Nibbles** | Zeile 1: `BIN: bb bbbb bbbb` (führende Nullen, 4er-Nibble-Gruppierung) | Intuitive Lesbarkeit von 10-Bit-Registern für Schulung & Diagnose |
| **🛑 Hold-Bedienlogik** | Taster an Pin `D4` (`INPUT_PULLUP`, Active LOW, Entprellung $\ge 50\,\text{ms}$) | Messwert wird nur bei Tastendruck aktualisiert und bis zum nächsten Druck eingefroren |
| **⚡ 70%-Sprung- & Plausibilitätsalarm** | Optischer LCD-Alarm bei relativem Sprung $\|\Delta\| \ge 70\,\%$ zum Vorgängerwert | Erkennt Sensorabrisse & Prozessanomalien; Nullpunktsschutz bei Werten nahe $0\,\text{V}$ |
| **🛡️ Watchdog & Auto-Recovery** | Software-Timeout bei Telegrammausfall $> 2{,}0\,\text{s}$ $\rightarrow$ LCD blinkt `ERR! TIMEOUT` | Fail-Safe-Verhalten; automatischer Wiederanlauf bei Signalrückkehr |
| **🌐 2× Interaktive Web-Sandboxen** | 1. Prozess-Simulation mit Live-Scope & Puffer<br>2. Physical Architecture Board | Vollständige Simulation im Browser & ideale Gesprächsgrundlage für Meetings |
| **🏛️ MBSE & Capella-Schnittstelle** | Standardisierter **OMG ReqIF 1.0** Export (`.reqif`) | Direkter Import in Eclipse Capella, IBM DOORS & Siemens Polarion |

---

## 📊 Aktueller Projektstand & Meilensteine

| Meilenstein / Phase | Status | Liefergegenstand / Artefakt |
| :--- | :---: | :--- |
| **1. Anforderungsanalyse (VDI 2206)** | 🟢 **Abgeschlossen** | [`Anforderungsspezifikation_Arduino_Teststand.md`](Anforderungsspezifikation_Arduino_Teststand.md) |
| **2. Hardware-Schaltplan & Pinbelegung** | 🟢 **Abgeschlossen** | [`Aufbauanleitung_und_Schaltplan.md`](Aufbauanleitung_und_Schaltplan.md) |
| **3. Firmware-Implementierung (UNO R4)** | 🟢 **Abgeschlossen** | Sketche in [`Sender_Datenerfassung/`](Sender_Datenerfassung/) & [`Empfaenger_Auswertung_HMI/`](Empfaenger_Auswertung_HMI/) |
| **4. Modulare Parameter-Konfiguration** | 🟢 **Abgeschlossen** | Zentrale Header-Datei [`TeststandConfig.h`](TeststandConfig.h) |
| **5. Prozess- & Plausibilitäts-Sandbox** | 🟢 **Einsatzbereit** | [`Teststand_Sandbox_Simulation.html`](Teststand_Sandbox_Simulation.html) |
| **6. Physisches Komponenten-Board** | 🟢 **Einsatzbereit** | [`Physische_Komponenten_Sandbox.html`](Physische_Komponenten_Sandbox.html) |
| **7. MBSE Modell-Export für Eclipse Capella**| 🟢 **Exportiert** | [`Anforderungen_Arduino_Teststand.reqif`](Anforderungen_Arduino_Teststand.reqif) & Anleitung |
| **8. Praktischer Laboraufbau** | 🟡 **Bereit für Labor** | Vorbereitet für den Teststandsaufbau nächste Woche |

---

## 🔌 Physische Architektur & Systemtopologie

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

### Pin-Verdrahtung im Überblick:
* **Sender-Arduino:** `A0` (Poti-Schleifer), `Pin 1` (TX Bus zu Empfänger Pin 0), `GND` (Gemeinsame Masse).
* **Empfänger-Arduino:** `Pin 0` (RX Bus von Sender TX), `Pin D4` (Hold-Taster gegen Masse), `A4` (I2C SDA), `A5` (I2C SCL), `GND` (Gemeinsame Masse).

---

## 📁 Repository-Struktur

```text
Projekt_Kirchner_Unterricht/
├── README.md                                       # Projektübersicht, Key Features & Status
├── .gitignore                                      # Ausschluss von Build-Artefakten
├── Anforderungsspezifikation_Arduino_Teststand.md  # Normgerechtes Lastenheft (VDI 2206 / VDI 2221)
├── Deterministischer_Automat_Dokumentation.md      # Formale DFA/FSM-Spezifikation (Zustandsautomat & Tabellen)
├── Aufbauanleitung_und_Schaltplan.md               # Pin-to-Pin Verdrahtungsplan & Laboranleitung
├── Capella_Import_Anleitung.md                     # Import- & Modellierungsanleitung für Eclipse Capella
├── Anforderungen_Arduino_Teststand.reqif           # OMG ReqIF 1.0 Modelldatei für Eclipse Capella & DOORS
├── Anforderungsprotokoll_Arduino_Teststand_v1.1.xlsx# Ursprüngliche Anforderungsmatrix
├── TeststandConfig.h                              # Zentrale Parameter-Headerdatei für beide Arduinos
├── Teststand_Sandbox_Simulation.html               # Interaktive Prozess- & Plausibilitäts-Sandbox
├── Physische_Komponenten_Sandbox.html              # Interaktives Physical Architecture Board
├── Sender_Datenerfassung/
│   ├── Sender_Datenerfassung.ino                  # Firmware für Datenerfassungs-Knoten (UNO R4)
│   └── TeststandConfig.h                          # Lokale Parameter-Kopie für Arduino IDE
└── Empfaenger_Auswertung_HMI/
    ├── Empfaenger_Auswertung_HMI.ino              # Firmware für Auswertungs- & HMI-Knoten (UNO R4)
    └── TeststandConfig.h                          # Lokale Parameter-Kopie für Arduino IDE
```

---

## ⚡ Schnellstart (Quickstart)

### 1. Interaktive Sandboxen starten (Keine Installation nötig)
* Öffnen Sie [`Teststand_Sandbox_Simulation.html`](Teststand_Sandbox_Simulation.html) im Browser zur Prozess- und Plausibilitätssimulation.
* Öffnen Sie [`Physische_Komponenten_Sandbox.html`](Physische_Komponenten_Sandbox.html) für das Hardware-Architektur-Board und die Besprechungsgrundlage.

### 2. Sketche auf Arduino UNO R4 flashen
1. In der Arduino IDE die Bibliothek **`LiquidCrystal I2C`** installieren.
2. [`Sender_Datenerfassung/Sender_Datenerfassung.ino`](Sender_Datenerfassung/Sender_Datenerfassung.ino) auf den Sender-UNO R4 hochladen.
3. [`Empfaenger_Auswertung_HMI/Empfaenger_Auswertung_HMI.ino`](Empfaenger_Auswertung_HMI/Empfaenger_Auswertung_HMI.ino) auf den Empfänger-UNO R4 hochladen.
4. Schaltung gemäß [`Aufbauanleitung_und_Schaltplan.md`](Aufbauanleitung_und_Schaltplan.md) verdrahten (wichtig: **GND mit GND verbinden**).

---

## 👥 Projektteam & Freigabe
* **Ausbildungsstätte:** Werner-von-Siemens-Schule Hildesheim – Fachschule Technik
* **Fachrichtung:** Elektrotechnik (2. Schuljahr)
* **Projektleitung / Dozent:** Herr Kirchner
* **Projektstatus:** Version 1.2 (Freigabe für Hardware-Teststand)
