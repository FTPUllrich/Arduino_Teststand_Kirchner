/**
 * @file Empfaenger_Auswertung_HMI.ino
 * @brief Auswertungs- & HMI-Knoten fuer 2-Arduino-Teststand (Arduino UNO R4)
 * @details Werner-von-Siemens-Schule Hildesheim - Fachschule Technik
 * 
 * Funktionen:
 * - UART-Datenempfang via Hardware-Serial1 (Pin 0 RX) mit Frame-Parser (<VAL:xxxx;RAW:xxxx>)
 * - Watchdog-Verbindungsueberwachung (Timeout > 2.0 s -> Fehlermeldung auf LCD)
 * - Taster-Erfassung an Pin D4 (INPUT_PULLUP, Entprellzeit >= 50 ms)
 * - Hold-Funktion: Aktualisierung des Anzeigewerts nur bei Tastendruck
 * - Plausibilitaets- und Sprungueberwachung (>= 70% Abweichung mit Nullpunktsschutz)
 * - 4-Zeilen-Ausgabe auf I2C LC-Display 20x4 (10-Bit-Binaerwert mit Nibbles, Dezimalwert, Status)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "TeststandConfig.h"

// =============================================================================
// HARDWARE-INSTANZEN & OBJEKTE
// =============================================================================

// LCD 20x4 via I2C (PCF8574 Backpack an SDA: A4, SCL: A5)
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

// =============================================================================
// SYSTEMVARIABLEN & STATUS
// =============================================================================

// Bus-Empfangspuffer & Werte
uint16_t latestReceivedAvg = 0;
uint16_t latestReceivedRaw = 0;
unsigned long lastTelegramTimestamp = 0;
bool isBusTimeout = true;
bool hasReceivedAnyData = false;

// Hold-Messwertspeicher
uint16_t displayedValue  = 0;
uint16_t previousValue   = 0;
float    deviationPct    = 0.0f;
bool     alarmState      = false;
bool     isFirstReading  = true;

// Taster-Verarbeitung (Entprellung & Flankenerkennung)
int           lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// LCD-Blink-Timer fuer Fehlermeldung
unsigned long lastBlinkTime = 0;
bool          blinkState    = false;

// =============================================================================
// HILFSFUNKTIONEN: FORMATIERUNG & BINAERDARSTELLUNG
// =============================================================================

/**
 * @brief Formatiert einen 10-Bit-Messwert (0..1023) in einen Binaerstring mit 4er-Nibbles
 * @param value 10-Bit Ganzzahlwert
 * @param buffer Ausgabepuffer (mind. 16 Bytes)
 * Format: "BIN: bb bbbb bbbb" (z.B. "BIN: 00 1111 1111")
 */
void formatBinary10Bit(uint16_t value, char* buffer) {
    // 10 Bits maskieren (Bit 9 bis Bit 0)
    char b[11];
    for (int i = 9; i >= 0; i--) {
        b[9 - i] = ((value >> i) & 0x01) ? '1' : '0';
    }
    b[10] = '\0';

    // Nibble-Trennung: Bit 9..8 | Bit 7..4 | Bit 3..0
    snprintf(buffer, 18, "BIN: %c%c %c%c%c%c %c%c%c%c",
             b[0], b[1],
             b[2], b[3], b[4], b[5],
             b[6], b[7], b[8], b[9]);
}

/**
 * @brief Aktualisiert den vollstaendigen Bildschirminhalt des 20x4 LCDs
 */
void updateLcdDisplay() {
    char lineBuffer[21];

    // -------------------------------------------------------------------------
    // Zeile 1: 10-Bit Binaeranzeige mit Nibble-Trennung
    // -------------------------------------------------------------------------
    lcd.setCursor(0, 0);
    formatBinary10Bit(displayedValue, lineBuffer);
    // Auf 20 Zeichen mit Leerzeichen auffuellen
    snprintf(lineBuffer, sizeof(lineBuffer), "%-20s", lineBuffer);
    lcd.print(lineBuffer);

    // -------------------------------------------------------------------------
    // Zeile 2: Dezimalwert und Filtertiefe
    // -------------------------------------------------------------------------
    lcd.setCursor(0, 1);
    snprintf(lineBuffer, sizeof(lineBuffer), "DEC: %4u (Avg:%02u)  ", displayedValue, FILTER_WINDOW_SIZE);
    lcd.print(lineBuffer);

    // -------------------------------------------------------------------------
    // Zeile 3: Abweichungsueberwachung (DEV) & Plausibilitaets-Alarm
    // -------------------------------------------------------------------------
    lcd.setCursor(0, 2);
    if (isFirstReading) {
        lcd.print(F("DEV:  INIT   [OK]   "));
    } else {
        char sign = (deviationPct >= 0.0f) ? '+' : '-';
        float absDev = abs(deviationPct);
        if (alarmState) {
            snprintf(lineBuffer, sizeof(lineBuffer), "DEV: %c%4.1f%% [ALARM]", sign, absDev);
        } else {
            snprintf(lineBuffer, sizeof(lineBuffer), "DEV: %c%4.1f%% [OK]   ", sign, absDev);
        }
        lcd.print(lineBuffer);
    }

    // -------------------------------------------------------------------------
    // Zeile 4: Betriebsmodus & Fehleranzeige
    // -------------------------------------------------------------------------
    lcd.setCursor(0, 3);
    if (isBusTimeout) {
        lcd.print(F("MOD: ERR! TIMEOUT   "));
    } else if (isFirstReading) {
        lcd.print(F("MOD: WAIT TASTE     "));
    } else {
        lcd.print(F("MOD: HOLD [TASTE]   "));
    }
}

/**
 * @brief Parst ein serielles Telegramm im Format: <VAL:xxxx;RAW:xxxx>
 */
bool parseTelegram(const char* frame) {
    unsigned int val = 0;
    unsigned int raw = 0;
    if (sscanf(frame, "<VAL:%4u;RAW:%4u>", &val, &raw) == 2) {
        latestReceivedAvg = (uint16_t)val;
        latestReceivedRaw = (uint16_t)raw;
        lastTelegramTimestamp = millis();
        isBusTimeout = false;
        hasReceivedAnyData = true;
        return true;
    }
    return false;
}

// =============================================================================
// SETUP ROUTINE
// =============================================================================
void setup() {
    // 1. Serielle Schnittstellen initialisieren
    DEBUG_SERIAL_PORT.begin(115200);
    BUS_SERIAL_PORT.begin(UART_BAUDRATE);

    // 2. Hardware-Taster mit internem Pull-Up konfigurieren (Active LOW)
    pinMode(PIN_BUTTON_HOLD, INPUT_PULLUP);

    // 3. I2C LCD initialisieren
    lcd.init();
    lcd.backlight();
    lcd.clear();

    // 4. Einschalt- / Boot-Screen anzeigen (VDI 2206 A-10)
    lcd.setCursor(0, 0);
    lcd.print(F("===================="));
    lcd.setCursor(0, 1);
    lcd.print(F("  TESTSTAND V1.2    "));
    lcd.setCursor(0, 2);
    lcd.print(F("  UNO R4 HMI-NODE   "));
    lcd.setCursor(0, 3);
    lcd.print(F("===================="));

    DEBUG_SERIAL_PORT.println(F("=================================================="));
    DEBUG_SERIAL_PORT.println(F("EMPFAENGER-NODE (HMI / Auswertung UNO R4) BEREIT"));
    DEBUG_SERIAL_PORT.println(F("=================================================="));

    delay(2000); // 2 Sekunden Boot-Meldung gemaess Lastenheft A-10

    lcd.clear();
    updateLcdDisplay();
}

// =============================================================================
// HAUPTSCHLEIFE (LOOP)
// =============================================================================
void loop() {
    unsigned long currentMillis = millis();

    // -------------------------------------------------------------------------
    // 1. SERIELLEN BUS EMPFANGEN & FRAME-PARSING (UART Serial1)
    // -------------------------------------------------------------------------
    static char rxBuffer[32];
    static uint8_t rxIndex = 0;
    static bool receivingFrame = false;

    while (BUS_SERIAL_PORT.available() > 0) {
        char inChar = (char)BUS_SERIAL_PORT.read();

        if (inChar == '<') {
            rxIndex = 0;
            receivingFrame = true;
            rxBuffer[rxIndex++] = inChar;
        } else if (receivingFrame) {
            if (rxIndex < sizeof(rxBuffer) - 1) {
                rxBuffer[rxIndex++] = inChar;
            }
            if (inChar == '>') {
                rxBuffer[rxIndex] = '\0';
                receivingFrame = false;

                if (parseTelegram(rxBuffer)) {
                    // Debug-Trace
                    DEBUG_SERIAL_PORT.print(F("[BUS RECV] Frame OK: "));
                    DEBUG_SERIAL_PORT.print(rxBuffer);
                    DEBUG_SERIAL_PORT.print(F(" -> AVG="));
                    DEBUG_SERIAL_PORT.println(latestReceivedAvg);
                }
            }
        }
    }

    // -------------------------------------------------------------------------
    // 2. TIMEOUT- & VERBINDUNGSUEBERWACHUNG (> 2000 ms)
    // -------------------------------------------------------------------------
    bool previousTimeoutState = isBusTimeout;
    if (currentMillis - lastTelegramTimestamp > BUS_TIMEOUT_MS) {
        isBusTimeout = true;
    }

    // Bei Statuswechsel oder im Timeout-Zustand Zeile 4 blinken lassen
    if (isBusTimeout) {
        if (currentMillis - lastBlinkTime >= 500) {
            lastBlinkTime = currentMillis;
            blinkState = !blinkState;
            lcd.setCursor(0, 3);
            if (blinkState) {
                lcd.print(F("MOD: ERR! TIMEOUT   "));
            } else {
                lcd.print(F("MOD:                "));
            }
        }
    } else if (previousTimeoutState == true && isBusTimeout == false) {
        // Auto-Recovery nach Verbindungswiederkehr
        DEBUG_SERIAL_PORT.println(F("[BUS STATUS] Verbindung wiederhergestellt."));
        updateLcdDisplay();
    }

    // -------------------------------------------------------------------------
    // 3. TASTER-ABFRAGE & HOLD-AKTUALISIERUNG (Pin D4, Active LOW)
    // -------------------------------------------------------------------------
    int reading = digitalRead(PIN_BUTTON_HOLD);

    // Flankenerkennung mit Software-Entprellung (50 ms)
    if (reading != lastButtonState) {
        lastDebounceTime = currentMillis;
    }

    if ((currentMillis - lastDebounceTime) > DEBOUNCE_TIME_MS) {
        static int buttonState = HIGH;
        if (reading != buttonState) {
            buttonState = reading;

            // Fallende Flanke: Taster wurde gedrueckt
            if (buttonState == LOW) {
                DEBUG_SERIAL_PORT.println(F("[TASTER] Hold-Trigger betaetigt!"));

                if (hasReceivedAnyData) {
                    if (isFirstReading) {
                        // Erster Tastendruck nach Kaltstart: Referenzwert ohne Alarm setzen
                        displayedValue = latestReceivedAvg;
                        previousValue  = latestReceivedAvg;
                        deviationPct   = 0.0f;
                        alarmState     = false;
                        isFirstReading = false;
                    } else {
                        // Nachfolgende Tastendruecke: Differenz & Plausibilitaet berechnen
                        previousValue  = displayedValue;
                        displayedValue = latestReceivedAvg;

                        int32_t diff = (int32_t)displayedValue - (int32_t)previousValue;

                        // Zweistufige mathematische Alarmpruefung (Lastenheft 4.2)
                        if (previousValue >= 10) {
                            deviationPct = ((float)diff / (float)previousValue) * 100.0f;
                            alarmState = (abs(deviationPct) >= ALARM_DEV_THRESHOLD_PCT);
                        } else {
                            // Rauschabsicherung bei Nullpunktwerten
                            deviationPct = ((float)diff / 10.0f) * 100.0f;
                            alarmState = (abs(diff) >= ALARM_MIN_ABS_DIFF);
                        }
                    }

                    // Display sofort aktualisieren (Latenz <= 100 ms)
                    updateLcdDisplay();

                    DEBUG_SERIAL_PORT.print(F("Neuer Anzeigewert: "));
                    DEBUG_SERIAL_PORT.print(displayedValue);
                    DEBUG_SERIAL_PORT.print(F(" | Vorher: "));
                    DEBUG_SERIAL_PORT.print(previousValue);
                    DEBUG_SERIAL_PORT.print(F(" | Abweichung: "));
                    DEBUG_SERIAL_PORT.print(deviationPct);
                    DEBUG_SERIAL_PORT.print(F("% | Alarm: "));
                    DEBUG_SERIAL_PORT.println(alarmState ? F("JA") : F("NEIN"));
                } else {
                    DEBUG_SERIAL_PORT.println(F("[WARNUNG] Noch keine Busdaten empfangen!"));
                }
            }
        }
    }
    lastButtonState = reading;
}
