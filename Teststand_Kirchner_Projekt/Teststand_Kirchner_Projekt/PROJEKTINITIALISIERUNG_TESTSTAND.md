# 📄 PROJEKTAUFTRAG & INITIALISIERUNGSDOKUMENT (PID)
**Nach DIN 69901 (Projektmanagement) & VDI 2206 (Entwicklungsmethodik mechatronischer Systeme)**  
**Projektbezeichnung:** Mikrocontroller-Teststand zur analogen Messwerterfassung, UART-Busübertragung & LCD-Plausibilitätsanzeige (2× Arduino UNO R4)  
**Projekt-ID:** `WFSE-25-TESTSTAND-01`  
**Datum der Initialisierung:** 07.09.2026 • **Status:** Formell initialisiert & freigabebereit  

---

## 1. Projekt-Steckbrief & Ausgangslage

| Attribut | Festlegung |
| :--- | :--- |
| **Projektname** | Gekoppelter 2× Arduino UNO R4 Mikrocontroller-Teststand |
| **Auftraggeber / Prüfer** | Herr Kirchner (Werner-von-Siemens-Schule Hildesheim) |
| **Ausbildungsbereich** | Fachschule Technik – Fachrichtung Elektrotechnik (WFSE-25) |
| **Normative Grundlagen** | VDI 2206 (Mechatronik), VDI 2221 (Entwicklungsmethodik), DIN 69901 (PM) |
| **Aktueller Meilenstein** | M1: Projektinitialisierung & Systemebene (Lastenheft V1.2) |
| **Ziel-Fertigstellung Teststand** | 25. September 2026 (Abschluss der Systemvalidierung im Labor) |
| **Offizieller Präsentationstermin** | 26. November 2026 (Kolloquium Janis Ole Stach laut `Liste_Termine_Themen_WFSE_25.xls`) |

### Problemstellung & Ausgangssituation:
In industriellen mechatronischen Anlagen müssen dezentral erfasste Sensorsignale (z. B. Drücke, Temperaturen, Dehnungen) zuverlässig analog digitalisiert, gefiltert und über Feldbusstrukturen an übergeordnete Bedien- und Auswerteeinheiten übertragen werden. Im Rahmen der Ausbildung soll ein mechatronischer Teststand realisiert werden, der diese Kette aus **Sensorik $\rightarrow$ Signalfilterung $\rightarrow$ deterministischer Buskommunikation $\rightarrow$ HMI & Plausibilitätsüberwachung** transparent und fehlersicher abbildet.

---

## 2. Projektziele (SMART-Kriterien)

Das Projekt wird anhand der fünf SMART-Kriterien verbindlich eingegrenzt:

1. **S – Spezifisch:**  
   Aufbau eines 2-Knoten-Teststands mit zwei getrennten Arduino UNO R4 Mikrocontrollern. Sender übernimmt 10-Bit ADC-Erfassung und gleitenden Mittelwert ($N=10$, $20\,\text{Hz}$); Empfänger übernimmt seriellen Empfang via Hardware-UART `Serial1`, Hold-Tastererfassung, 70%-Sprungüberwachung und 20×4 I2C-LC-Display-Ansteuerung.
2. **M – Messbar:**  
   * Linearitätsfehler des ADC $\le \pm 2\,\text{LSB}$ über $0 \dots 5\,\text{V}$ DC.
   * Buszyklus $f_{\text{bus}} \ge 10\,\text{Hz}$ ($\Delta t \le 100\,\text{ms}$) bei $9600\,\text{Baud}$.
   * Watchdog-Timeout: Autonome Fehleranzeige nach $t > 2{,}0\,\text{s}$ ohne Signal.
   * Latenzzeit Taster $\rightarrow$ Anzeige $t_{\text{lat}} \le 100\,\text{ms}$.
   * Optischer Alarm bei relativer Sollwertabweichung $|\Delta_{\text{rel}}| \ge 70{,}0\,\%$ (Absolutgrenze $|\Delta| \ge 50\,\text{Digits}$ bei Referenz $<10\,\text{Digits}$).
3. **A – Akzeptiert & Attraktiv:**  
   Abgestimmt auf die 11 Qualitätskriterien der Schulcheckliste von Herrn Kirchner. Bereitstellung eines browserbasierten Simulators zur Vorab-Validierung und eines MBSE-Exports (OMG ReqIF 1.0).
4. **R – Realistisch:**  
   Vollständig realisierbar mit Standard-Labormaterialien (2× UNO R4, 10k-Potentiometer, HD44780 20x4 LCD mit PCF8574 Backpack, Steckbrett, Taster).
5. **T – Terminiert:**  
   * **08.09.2026:** Abstimmung und Lastenheft-Freigabe (Lehrergespräch).
   * **15.09.2026:** Abschluss Hardwareaufbau & Flashen der Firmware.
   * **22.09.2026:** Abschluss Integrationstests (TC-01 bis TC-06).
   * **25.09.2026:** Finale Systemvalidierung (TC-07) & Projektabnahme im Labor.
   * **26.11.2026:** Fachvortrag & Abschlusspräsentation (Janis Ole Stach, WFSE-25).

---

## 3. Projektabgrenzung (In-Scope / Out-of-Scope)

Um Verzögerungen und Streitigkeiten zu vermeiden, ist der Projektumfang vertraglich abgegrenzt:

```
+---------------------------------------------------------------------------------------------------------+
|                                           PROJEKT-SCOPE MATRIX                                          |
+---------------------------------------------------------------------------------------------------------+
| [IN-SCOPE] Was verbindlich geschuldet wird:                                                             |
|   • Auslegung und Verdrahtung beider Arduino UNO R4 Minima Knoten auf Labor-Steckbrett                  |
|   • Signalglättung via 10-Elemente-FIFO-Ringpuffer mit Kaltstart-Vorbefüllung                            |
|   • Punkt-zu-Punkt Hardware-UART-Kommunikation mit Telegramm <VAL:xxxx;RAW:xxxx>                       |
|   • Entprellter Hold-Bedientaster (INPUT_PULLUP, t_deb >= 50ms)                                         |
|   • 20x4 LCD-Darstellung mit 10-Bit Binärwerten in 4er-Nibbles (BIN: bb bbbb bbbb)                      |
|   • Autonomer Watchdog mit Reconnect-Funktion                                                           |
|   • Vollständige VDI 2206 Dokumentation, Traceability-Matrix & ReqIF-Modell                             |
+---------------------------------------------------------------------------------------------------------+
| [OUT-OF-SCOPE] Was NICHT Bestandteil dieses Projekts ist:                                               |
|   • Entwicklung eigener geätzter Leiterplatten (PCB-Fertigung) -> Es wird Steckbrett genutzt            |
|   • Industriegehäuse mit IP65/IP67 -> Betriebsumgebung ist Labor (IP20)                                 |
|   • Mehrkanal-Erweiterung A1..A5 in Phase 1 -> Ist als Wunschforderung (W) für Phase 2 reserviert       |
|   • Cloud-Anbindung / IoT-Übertragung (WiFi/MQTT) -> Rein lokale Buskopplung via UART                   |
+---------------------------------------------------------------------------------------------------------+
```

---

## 4. Projektorganisation & Rollenverteilung

| Rolle im Projekt | Verantwortungsbereich | Zuständig |
| :--- | :--- | :--- |
| **Auftraggeber & Prüfer** | Fachliche Vorgaben, Zwischenabnahmen, Endabnahme | Herr Kirchner |
| **Projektleitung & Systemtechnik** | Gesamtkoordination, VDI 2206 V-Modell, Lastenheft-Führung, MBSE (ReqIF) | Projektteam |
| **Hardware- & Elektronik-Lead** | Schaltungsaufbau, Pin-Verdrahtung, Pegelprüfung, Entstörung, Oszilloskop-Messung | Projektteam |
| **Firmware- & Software-Lead** | C++ Entwicklung (Sender.ino, Empfaenger.ino, TeststandConfig.h), UART-Parser, LCD-Treiber | Projektteam |
| **Qualitäts- & Testmanager** | Durchführung der Verifikationsmatrix (TC-01 bis TC-07), Testprotokollführung | Projektteam |

---

## 5. Materialliste & Ressourcen (Bill of Materials - BOM)

| Pos. | Bauteil / Komponente | Spezifikation | Einsatzort | Status |
| :---: | :--- | :--- | :--- | :---: |
| **1** | **Mikrocontroller 1** | Arduino UNO R4 Minima (Renesas RA4M1 48 MHz) | Sender-Knoten (Datenerfassung) | Verfügbar |
| **2** | **Mikrocontroller 2** | Arduino UNO R4 Minima (Renesas RA4M1 48 MHz) | Empfänger-Knoten (HMI & Logik) | Verfügbar |
| **3** | **Signalgeber (Sensor)** | Lineares Dreh-Potentiometer $10\,\text{k}\Omega$ | Messgröße an Sender Pin A0 | Verfügbar |
| **4** | **LC-Display** | HD44780 20×4 Textdisplay | HMI-Ausgabe am Empfänger | Verfügbar |
| **5** | **I2C-Backpack** | PCF8574 I2C Expander (Adr. `0x27` / `0x3F`) | 2-Draht-Anbindung LCD Pin A4/A5 | Verfügbar |
| **6** | **Bedientaster** | Kurzhub-Taster (Schließer, Active LOW) | Hold-Trigger an Empfänger Pin D4 | Verfügbar |
| **7** | **Labor-Steckbrett** | Standard Breadboard (mind. 830 Kontakte) | Montage & Signalverdrahtung | Verfügbar |
| **8** | **Verbindungskabel** | Jumper Wires (M/M, M/F) | TX/RX, I2C, VCC & GND-Brücke | Verfügbar |
| **9** | **Messmittel Labor** | 2-Kanal-Digital-Speicheroszilloskop, Digitalmultimeter | Verifikation nach TC-01..TC-07 | Im Schullabor |

---

## 6. Initiales Risikomanagement (FMEA-Vorschau)

| Risiko-ID | Mögliches Risiko / Fehlerursache | Auswirkung auf das Projekt | Eintritts-Wsk. | Schwere | Geplante Gegenmaßnahme |
| :---: | :--- | :--- | :---: | :---: | :--- |
| **R-01** | **Gemeinsame Masse (GND) nicht verbunden** | UART-Übertragung fehlerhaft oder Pegelverschiebung führt zu Signalverlust | Mittel | Hoch | **Festforderung A-14:** Zwingende GND-Brücke im Schaltplan rot markiert und vor dem Einschalten per DMM gemessen. |
| **R-02** | **Konflikt zwischen USB und UART** | Serieller Monitor stört Busverbindung zwischen Arduinos | Gering | Hoch | **UNO R4 Architektur:** Nutzung von Hardware-UART `Serial1` für den Bus; USB `Serial` bleibt vollkommen unabhängig. |
| **R-03** | **Kontaktprellen am Hold-Taster** | Mehrfaches ungewolltes Aktualisieren des Messwerts | Hoch | Mittel | **Software-Entprellung:** Nicht-blockierender Timer mit $t_{\text{deb}} \ge 50\,\text{ms}$ in `Empfaenger.ino`. |
| **R-04** | **Mathematische Division durch 0** | Absturz oder falscher Alarm bei Geberstellung nahe $0\,\text{V}$ | Mittel | Hoch | **Nullpunktschutz (A-11):** Bei Referenzwert $<10\,\text{Digits}$ greift feste Mindestdifferenz $|\Delta| \ge 50\,\text{Digits}$. |
| **R-05** | **Leitungsabriss im Betrieb** | Empfänger friert ein und zeigt veralteten Zustand | Mittel | Hoch | **Watchdog-Timer (A-05):** Autonome Umschaltung auf `ERR: COMM TIMEOUT!` nach $>2{,}0\,\text{s}$ Signalverlust. |

---

## 7. Definition of Ready (DoR) & Definition of Done (DoD)

### Definition of Ready (Startvoraussetzung für den physischen Aufbau):
* [x] Vollständiges, auditiertes Lastenheft Version 1.2 liegt vor.
* [x] Die 11 Qualitätskriterien der Schulcheckliste sind nachweisbar erfüllt.
* [x] Schaltplan und Pinbelegungen sind eindeutig dokumentiert.
* [x] Firmware für Sender und Empfänger ist vorab simuliert und fehlerfrei.
* [x] Bauteile (UNO R4, Poti, LCD, Taster) sind vollständig vorhanden.

### Definition of Done (Endabnahme für das Gesamtprojekt):
* [ ] Hardware ist sauber auf dem Breadboard verdrahtet (inkl. gemeinsamer GND-Brücke).
* [ ] Sender erfasst den vollen Bereich ($0 \dots 1023\,\text{Digits}$) und filtert Rauschen sichtbar.
* [ ] Empfänger zeigt 10-Bit Binärwert in 4er-Nibbles (`BIN: bb bbbb bbbb`) und Dezimalwert an.
* [ ] Hold-Taster friert Messwert zuverlässig ein und aktualisiert prellfrei.
* [ ] Plausibilitätsalarm löst bei $|\Delta| \ge 70\,\%$ zuverlässig aus und setzt sich bei Rückkehr zurück.
* [ ] Timeout von $>2{,}0\,\text{s}$ schaltet autonome Fehlermeldung auf Zeile 4 des Displays.
* [ ] Das Abnahmeprotokoll der Testfälle TC-01 bis TC-07 ist durch Herrn Kirchner gegengezeichnet.

---

## 8. Formelle Freigabeerklärung

Mit den nachfolgenden Unterschriften wird das Projekt **WFSE-25-TESTSTAND-01** auf Basis des Lastenhefts Version 1.2 formell initialisiert und für die Realisierungsphase freigegeben:


Hildesheim, den \_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_



\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_                    \_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_  
**Projektteam (Auftragnehmer)**                               **Herr Kirchner (Auftraggeber / Prüfer)**
