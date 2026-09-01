# IMPORT-ANLEITUNG FÜR ECLIPSE CAPELLA (MBSE)
## Projekt: Mikrocontroller-Teststand (2× Arduino UNO R4)
### Format: OMG ReqIF 1.0 (Requirements Interchange Format)

---

## 1. Übersicht der Importdatei

Die generierte Datei **[`Anforderungen_Arduino_Teststand.reqif`](file:///c:/Users/Janis%20Ole/Downloads/Projekt_Kirchner_Unterricht/Anforderungen_Arduino_Teststand.reqif)** enthält alle 15 Systemanforderungen (A-01 bis A-15) im offiziellen **OMG ReqIF-Standard (Requirements Interchange Format)**, dem universellen Industrie-Schnittstellenstandard für Eclipse Capella, IBM DOORS, Siemens Polarion und PTC Codebeamer.

### Enthaltene Attribute je Anforderung:
* **Req-ID:** `A-01` bis `A-15`
* **Kategorie:** z. B. `Sensorik & Erfassung`, `Kommunikation`, `Logik & Alarm`
* **Titel & Spezifikation:** Vollständige technische Beschreibung
* **Zielwert / Akzeptanzkriterium:** Messbare Grenzwerte und Zielvorgaben
* **Verifikationsmethode:** Zuordnung zu den Testfällen (TC-01 bis TC-07)
* **Klassifizierung:** `Festforderung (F)` oder `Wunschforderung (W)`
* **Priorität:** `Hoch`, `Mittel`, `Niedrig`

---

## 2. Schritt-für-Schritt Import in Eclipse Capella

### Voraussetzung in Eclipse Capella:
In Capella wird das offizielle Add-on **Requirements Viewpoint** (`org.polarsys.capella.vp.requirements`) genutzt (in den meisten Capella-Installationen standardmäßig enthalten oder über den Extension Manager installierbar).

### Import-Ablauf:
1. **Capella öffnen** und Ihr Projekt im *Project Explorer* auswählen (oder ein neues Capella-Projekt anlegen: `File` $\rightarrow$ `New` $\rightarrow$ `Capella Project`).
2. Klicken Sie im Hauptmenü auf:
   $$\text{File} \longrightarrow \text{Import...}$$
3. Wählen Sie im Import-Dialog:
   $$\text{Requirements} \longrightarrow \text{Import ReqIF (oder ReqIF Document)}$$
4. Wählen Sie als Quelldatei (**Source File**):
   `c:\Users\Janis Ole\Downloads\Projekt_Kirchner_Unterricht\Anforderungen_Arduino_Teststand.reqif`
5. Wählen Sie als Zielordner Ihr Capella-Projekt aus und klicken Sie auf **Finish**.
6. **Ergebnis:** Im Projektbaum erscheint ein neuer Ordner `Requirements` mit dem Modul *„Systemanforderungen Arduino Teststand“* und allen 15 Einzelanforderungen.

---

## 3. Empfohlene Modellierung nach der ARCADIA-Methode in Capella

In Eclipse Capella können Sie diese Anforderungen nun direkt per Drag & Drop mit den Modellelementen der ARCADIA-Phasen verknüpfen (**Traceability**):

```
+---------------------------------------------------------------------------------------------------------+
|                                ARCADIA-EBENEN IN ECLIPSE CAPELLA                                        |
|                                                                                                         |
| 1. SYSTEM ANALYSIS (SA)                                                                                 |
|    • System Function: "Erfasse Analogsignal", "Filtere Rauschen", "Übertrage Bus", "Visualisiere Binär"|
|    • Actor: "Benutzer (Bediener)", "Analogsensor (Geber)"                                               |
|    • Verknüpfung: Anforderungen A-01, A-02, A-07, A-08, A-10                                            |
|                                                                                                         |
| 2. LOGICAL ARCHITECTURE (LA)                                                                            |
|    • Logical Component 1: "LC_Datenerfassung"                                                           |
|    • Logical Component 2: "LC_Auswertung_HMI"                                                           |
|    • Logical Exchange: "UART_DataStream <VAL;RAW>" (A-04, A-05)                                         |
|    • Verknüpfung: Anforderungen A-04, A-05, A-11, A-12, A-13                                            |
|                                                                                                         |
| 3. PHYSICAL ARCHITECTURE (PA)                                                                           |
|    • Physical Component 1: "Arduino UNO R4 Minima (Sender)"                                             |
|    • Physical Component 2: "Arduino UNO R4 Minima (Empfänger)"                                          |
|    • Physical Component 3: "LCD 20x4 I2C Backpack (PCF8574)"                                            |
|    • Physical Component 4: "Potentiometer 10k" & "Hold-Taster (D4)"                                     |
|    • Physical Links: "UART Serial1 (5V TTL)", "I2C Bus (100 kHz)"                                       |
|    • Verknüpfung: Anforderungen A-06, A-14, A-15                                                        |
+---------------------------------------------------------------------------------------------------------+
```
