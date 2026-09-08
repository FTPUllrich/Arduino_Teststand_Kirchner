# 📋 AUDIT & KONSEQUENZEN AUS DER ANFORDERUNGSCHECKLISTE
## Projekt: 2× Arduino UNO R4 Mikrocontroller-Teststand
### Werner-von-Siemens-Schule Hildesheim – Fachschule Technik Elektrotechnik
*Referenzdokument: `Anforderungscheckliste.docx` (Schulstandard Herr Kirchner)*

---

## 1. Übersicht des Kriterien-Audits

Die neu bereitgestellte **`Anforderungscheckliste.docx`** definiert 11 verbindliche Qualitätskriterien für professionelle und prüfbare Anforderungsspezifikationen (Lastenhefte). 

Das folgende Audit analysiert unseren Anforderungskatalog (A-01 bis A-15) anhand aller 11 Kriterien und leitet die konkreten **sprachlichen und strukturellen Konsequenzen** ab:

---

## 2. Detaillierte Kriterien-Prüfung & Konsequenzen

```
+---------------------------------------------------------------------------------------------------------+
|                                    DIE 11 KRITERIEN IM DETAIL-AUDIT                                     |
+---------------------------------------------------------------------------------------------------------+
```

### 1. Eindeutige Referenzgrößen (Keine vagen Adjektive wie „schnell“, „hoch“, „groß“)
* **Checkliste:** Vage Begriffe sind nicht überprüfbar. *Beispiel: „ab 130 km/h“ statt „bei hohen Geschwindigkeiten“.*
* **Status im Projekt:** Bereits sehr gut quantifiziert.
* **Gezogene Konsequenz:**
  * Alle Schwellenwerte besitzen zwingend numerische Toleranzbänder:
    * Abtastung: $\Delta t = 50\,\text{ms} \pm 2\,\text{ms}$ ($20\,\text{Hz}$).
    * Bus-Latenz: $t_{\text{lat}} \le 100\,\text{ms}$ zwischen Tastendruck und Displayanzeige.
    * Entprellzeit: $t_{\text{deb}} \ge 50\,\text{ms}$.
    * Timeout: $t_{\text{timeout}} > 2{,}0\,\text{s}$.
    * Plausibilität: Relativer Sprung $\ge 70{,}0\,\%$, Absolutsprung nahe $0\,\text{V}$ $\ge 50\,\text{Digits}$.

---

### 2. Atomarität (Eine Anforderung = Genau eine prüfbare Aussage)
* **Checkliste:** Eine Anforderung enthält genau eine Aussage – sonst lässt sie sich nicht einzeln prüfen oder ändern.
* **Gezogene Konsequenz:**
  * **A-11 (Alarmerkennung)** und **A-12 (Alarmquittierung)** bleiben strikt getrennt: A-11 regelt rein die *Erkennung und Signalisierung*, A-12 rein das *Rücksetzverhalten*.
  * **A-08 (Hold-Funktion)** trennt funktionale Messwertübernahme von der elektromechanischen Prellunterdrückung.

---

### 3. Eindeutige Kennzeichnung (ID) & Rückverfolgbarkeit
* **Checkliste:** Ermöglicht Rückverfolgbarkeit (Traceability) beim Testen oder bei Änderungswünschen (`ANF-001` / `A-01` etc.).
* **Gezogene Konsequenz:**
  * Jede Anforderung behält ihre eindeutige ID `A-01` bis `A-15`.
  * Jede Anforderung wird in der Verifikationsmatrix 1-zu-1 einem dedizierten Testfall (`TC-01` bis `TC-07`) zugeordnet.

---

### 4. Verständliche Sprache & Glossar (Kein unerklärter Fachjargon)
* **Checkliste:** Unerklärte Anglizismen/Fachbegriffe sind für Kunde und Team nicht eindeutig. *Beispiel: „MFD“, „ACK-Package“ vermeiden oder definieren.*
* **Gezogene Konsequenz:**
  * Im Lastenheft wird ein **verbindliches Glossar (Kapitel 1.3)** eingefügt, das alle verwendeten Fachbegriffe für Prüfer und Dozenten eindeutig definiert:
    * **Hold-Modus:** Statisches Einfrieren des Messwerts auf der Anzeige.
    * **Moving Average:** Zeitdiskreter gleitender arithmetischer Mittelwert.
    * **Nibble:** 4-Bit Halbbyte zur visuellen Strukturierung von Binärregistern.
    * **Watchdog-Timeout:** Zeitüberwachungsfunktion zur Erkennung von Kommunikationsunterbrechungen.
    * **Auto-Recovery:** Selbstständige Wiederaufnahme des Normalbetriebs nach Fehlerbehebung.

---

### 5. „WAS statt WIE“ (Ergebnis statt Implementierungsdetail im Lastenheft)
* **Checkliste:** Anforderungen beschreiben das gewünschte Ergebnis (Systemverhalten), keine Implementierung. *Beispiel: „ohne Nachkommastellen anzeigen“ statt „Datentyp int“.*
* **Gezogene Konsequenz (Sehr wichtig für A-06, A-08, A-13):**
  * ❌ *Alt (WIE):* „A-13: Daten im SRAM über Pointer ablegen.“  
    👉 **Neu (WAS):** **A-13 (Messwert- und Referenzwertpufferung):** „Das System muss den aktuellen Messwert sowie den unmittelbar vorhergehenden Referenzwert zur Laufzeit kontinuierlich im Arbeitsspeicher vorhalten, um die Differenzbildung und Plausibilitätsprüfung verzögerungsfrei durchzuführen.“
  * ❌ *Alt (WIE):* „A-08: Taster an Pin D4 mit INPUT_PULLUP einlesen.“  
    👉 **Neu (WAS):** **A-08 (Hold-Messwertübernahme):** „Das System darf die Messwertanzeige ausschließlich bei Betätigung des Bedientasters aktualisieren und muss den angezeigten Wert bis zur nächsten Tastenbetätigung unverändert einfrieren. Prellzeiten von mindestens $50\,\text{ms}$ müssen sicher unterdrückt werden.“
  * ❌ *Alt (WIE):* „A-06: LCD über PCF8574 I2C Backpack ansteuern.“  
    👉 **Neu (WAS):** **A-06 (Messwert-Visualisierung):** „Das System muss alle Messwerte und Betriebszustände auf einer 4-zeiligen Monospace-Textanzeige mit mindestens 20 Zeichen pro Zeile über einen synchronen 2-Draht-Seriell-Bus darstellen.“

---

### 6. Vollständigkeit (Auch Ausnahme- & Fehlerfälle definieren)
* **Checkliste:** Auch Ausnahme-/Fehlerfälle und Folgeaktionen müssen definiert sein. *Beispiel: Was passiert nach einer erkannten Verbindungsstörung?*
* **Gezogene Konsequenz:**
  * **Signalverlust:** Tritt für $> 2{,}0\,\text{s}$ kein Telegramm ein, schaltet das System in den Fail-Safe-Modus (Zeile 4 blinkt: `MOD: ERR! TIMEOUT`).
  * **Auto-Recovery:** Sobald wieder gültige Daten empfangen werden, wird der Fehlerzustand selbstständig zurückgesetzt.
  * **Nullpunkt-Grenzfall ($Wert_{k-1} < 10\,\text{Digits}$):** Verhindert Division durch Null durch Umschaltung auf die absolute Signalschwelle ($\|\Delta\| \ge 50\,\text{Digits}$).
  * **Kaltstart ($k=1$):** Der allererste Tastendruck dient als Referenzwert-Initialisierung und löst keinen Fehlalarm aus.

---

### 7. Nachprüfbarkeit / Testbarkeit (Messbare Kriterien)
* **Checkliste:** Nur was messbar ist, kann später im Abnahmetest geprüft werden. *Beispiel: „unplausibel“ konkret definieren.*
* **Gezogene Konsequenz:**
  * Jede Anforderung enthält eine tabellarische Spalte **„Zielwert / Akzeptanzkriterium“** mit exakter Messmethode (z. B. 2-Kanal-Oszilloskop, Digitalmultimeter, Logikanalysator).

---

### 8. Redundanzfreiheit (Keine nichtssagenden Füllsätze)
* **Checkliste:** Triviale/nichtssagende Aussagen erschweren das Lesen ohne Mehrwert. *Beispiel: „Berechnung erfolgt computergestützt“ streichen.*
* **Gezogene Konsequenz:**
  * Reine Floskeln („das Programm läuft stabil“, „die Software ist performant“) wurden vollständig aus den Spezifikationstexten entfernt.

---

### 9. Konsistente thematische Struktur
* **Checkliste:** Thematisch geordnete Anforderungen sind leichter zu prüfen und zu pflegen.
* **Gezogene Konsequenz:**
  * Strukturierung der 15 Anforderungen in 7 logische Systemblöcke:
    1. *Sensorik & Signalerfassung* (A-01, A-03)
    2. *Signalverarbeitung & Filterung* (A-02)
    3. *Bus-Kommunikation & Schnittstellen* (A-04)
    4. *HMI, Display & Bedienung* (A-06, A-07, A-08, A-09)
    5. *Plausibilitäts- & Alarmüberwachung* (A-11, A-12)
    6. *Systemrobustheit & Initialisierung* (A-05, A-10, A-13)
    7. *Elektrik & Umgebungsbedingungen* (A-14, A-15)

---

### 10. Widerspruchsfreiheit
* **Checkliste:** Widersprechen sich Angaben innerhalb einer Anforderung, ist unklar, was gilt.
* **Gezogene Konsequenz:**
  * Alle Wertebereiche ($0 \dots 1023\,\text{Digits}$, $0{,}00 \dots 5{,}00\,\text{V}$, $10\,\text{Bit}$) und Taktraten ($20\,\text{Hz}$ Erfassung, $10\,\text{Hz}$ Bus) sind im gesamten Lastenheft, in den Header-Dateien (`TeststandConfig.h`) und im Automatenmodell absolut konsistent synchronisiert.

---

### 11. Konkretheit (Kein Blankoscheck wie „entsprechend reagieren“)
* **Checkliste:** Allgemeine Floskeln ohne konkrete Handlungsanweisung sind nicht prüfbar.
* **Gezogene Konsequenz:**
  * Jede Systemreaktion ist exakt formuliert:
    * Bei Plausibilitätsverletzung: Textwechsel in LCD-Zeile 3 von `[OK]` auf `[ALARM]`.
    * Bei Verbindungsabbruch: Alternierendes Blinken von `MOD: ERR! TIMEOUT` auf Zeile 4 im 500-ms-Takt.

---

## 3. Zusammenfassung der durchgeführten Aktualisierungen

1. **Lastenheft (`Anforderungsspezifikation_Arduino_Teststand.md`):**  
   Vollständig überarbeitet gemäß den 11 Kriterien, inklusive Glossar und strikter „WAS statt WIE“-Formulierung.
2. **Capella ReqIF (`Anforderungen_Arduino_Teststand.reqif`):**  
   Mit den geschärften Spezifikationen und Akzeptanzkriterien synchronisiert.
3. **Tabellen & Portale (`Anforderungen_Tabelle.csv`, Web-Portale):**  
   Texte aktualisiert und auf GitHub bereitgestellt.
