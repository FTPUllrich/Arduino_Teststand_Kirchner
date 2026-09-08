/**
 * @file Empfaenger.ino
 * @brief Empfänger-Knoten: UART-Bus-Empfang, Watchdog-Timeout, Hold-Logik, 70%-Plausibilität & I2C-LCD (20x4)
 * @details Werner-von-Siemens-Schule Hildesheim - 2x Arduino Mikrocontroller-Teststand
 *          Kompatibel mit Arduino UNO R4 (Minima/WiFi) & Arduino UNO R3
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "TeststandConfig.h"

// Hardware-UART Zuweisung je nach Board
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  #define BUS_SERIAL    Serial1  // Hardware-UART an Pin 0 (RX) & Pin 1 (TX)
  #define DEBUG_SERIAL  Serial   // USB-Schnittstelle zum PC
#else
  #define BUS_SERIAL    Serial   // UNO R3 Standard-Serial
  #define DEBUG_SERIAL  Serial
#endif

// I2C-LC-Display (20 Zeichen x 4 Zeilen)
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

// Messwert-Variablen
uint16_t latestBusVal = 0;       // Zuletzt über Bus empfangener Live-Wert
uint16_t latestRawVal = 0;       // Zuletzt über Bus empfangener Rohwert
uint16_t displayedVal = 0;       // Eingefrorener Anzeigewert (Hold)
uint16_t previousVal  = 0;       // Referenzwert vor dem letzten Tastendruck
float deviationPercent = 0.0f;

// Statusflags
bool hasReceivedData = false;
bool isFirstPress    = true;
bool isAlarmActive   = false;
bool isTimeoutActive = false;
bool wasTimeout      = false;

// Zeitüberwachung (Watchdog & Entprellung)
unsigned long lastDataTime = 0;
unsigned long lastDebounceTime = 0;
int lastButtonState = HIGH;

// Serieller Empfangspuffer
char rxBuffer[48];
uint8_t rxIndex = 0;

void format10BitBin(uint16_t val, char* outBuf) {
  // Format: "BIN: bb bbbb bbbb" (10-Bit mit 4er-Nibbles)
  outBuf[0] = (val & (1 << 9)) ? '1' : '0';
  outBuf[1] = (val & (1 << 8)) ? '1' : '0';
  outBuf[2] = ' ';
  outBuf[3] = (val & (1 << 7)) ? '1' : '0';
  outBuf[4] = (val & (1 << 6)) ? '1' : '0';
  outBuf[5] = (val & (1 << 5)) ? '1' : '0';
  outBuf[6] = (val & (1 << 4)) ? '1' : '0';
  outBuf[7] = ' ';
  outBuf[8] = (val & (1 << 3)) ? '1' : '0';
  outBuf[9] = (val & (1 << 2)) ? '1' : '0';
  outBuf[10] = (val & (1 << 1)) ? '1' : '0';
  outBuf[11] = (val & (1 << 0)) ? '1' : '0';
  outBuf[12] = '\0';
}

void renderDisplay() {
  char binStr[16];
  format10BitBin(displayedVal, binStr);

  // Zeile 1: 10-Bit Binärwert (A-07)
  lcd.setCursor(0, 0);
  lcd.print(F("BIN: "));
  lcd.print(binStr);
  lcd.print(F("   "));

  // Zeile 2: Dezimalwert & Filterinfo
  lcd.setCursor(0, 1);
  lcd.print(F("DEC: "));
  if (displayedVal < 1000) lcd.print(F(" "));
  if (displayedVal < 100)  lcd.print(F(" "));
  if (displayedVal < 10)   lcd.print(F(" "));
  lcd.print(displayedVal);
  lcd.print(F(" (Avg:10)  "));

  // Zeile 3: Abweichung & Plausibilitätsstatus (A-11, A-12)
  lcd.setCursor(0, 2);
  if (isFirstPress) {
    lcd.print(F("DEV:  INIT   [OK]   "));
  } else {
    lcd.print(F("DEV: "));
    if (deviationPercent >= 0.0f) lcd.print(F("+"));
    lcd.print(deviationPercent, 1);
    lcd.print(F("% "));
    if (isAlarmActive) {
      lcd.print(F("[ALARM!]"));
    } else {
      lcd.print(F("[OK]    "));
    }
  }

  // Zeile 4: Modus / Timeout-Meldung (A-05, A-08, A-10)
  lcd.setCursor(0, 3);
  if (isTimeoutActive) {
    lcd.print(F("ERR: COMM TIMEOUT!  "));
  } else if (isFirstPress) {
    lcd.print(F("MOD: WAIT TASTE     "));
  } else {
    lcd.print(F("MOD: HOLD [TASTE]   "));
  }
}

void processIncomingLine(const char* line) {
  // Erkennt Format: <VAL:0400;RAW:0400> oder VAL:0400
  uint16_t v = 0;
  uint16_t r = 0;

  if (sscanf(line, "<VAL:%hu;RAW:%hu>", &v, &r) >= 1) {
    latestBusVal = v;
    latestRawVal = r;
    lastDataTime = millis();
    hasReceivedData = true;
  } else if (sscanf(line, "VAL:%hu", &v) == 1) {
    latestBusVal = v;
    latestRawVal = v;
    lastDataTime = millis();
    hasReceivedData = true;
  }
}

void setup() {
  BUS_SERIAL.begin(UART_BAUDRATE);
  pinMode(PIN_BUTTON_HOLD, INPUT_PULLUP);

  #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.println(F("[EMPFAENGER] Arduino UNO R4 HMI initialisiert."));
  #endif

  // LCD initialisieren
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("TESTSTAND INIT...   "));
  lcd.setCursor(0, 1);
  lcd.print(F("WFSE-25 KIRCHNER    "));
  lcd.setCursor(0, 2);
  lcd.print(F("VDI 2206 / UNO R4   "));
  lcd.setCursor(0, 3);
  lcd.print(F("WARTE AUF BUS...    "));
  delay(1500);
  lcd.clear();

  renderDisplay();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Nicht-blockierender serieller Empfang (UART)
  while (BUS_SERIAL.available() > 0) {
    char c = (char)BUS_SERIAL.read();
    if (c == '\n' || c == '\r') {
      if (rxIndex > 0) {
        rxBuffer[rxIndex] = '\0';
        processIncomingLine(rxBuffer);
        rxIndex = 0;
      }
    } else {
      if (rxIndex < sizeof(rxBuffer) - 1) {
        rxBuffer[rxIndex++] = c;
      }
    }
  }

  // 2. Watchdog: Timeout-Prüfung (> 2.0 s nach A-05)
  if (hasReceivedData && (currentMillis - lastDataTime > BUS_TIMEOUT_MS)) {
    if (!isTimeoutActive) {
      isTimeoutActive = true;
      renderDisplay(); // Sofortige Aktualisierung bei Ausfall
    }
  } else {
    if (isTimeoutActive) {
      isTimeoutActive = false;
      renderDisplay(); // Sofortige Wiederherstellung bei Signalrückkehr
    }
  }

  // 3. Hold-Bedientaster mit Entprellung (A-08, A-09: >= 50ms)
  int reading = digitalRead(PIN_BUTTON_HOLD);
  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) >= DEBOUNCE_TIME_MS) {
    // Tastendruck (Fallende Flanke bei INPUT_PULLUP)
    if (reading == LOW && lastButtonState == HIGH) {
      if (hasReceivedData) {
        if (isFirstPress) {
          displayedVal = latestBusVal;
          previousVal = latestBusVal;
          deviationPercent = 0.0f;
          isAlarmActive = false;
          isFirstPress = false;
        } else {
          previousVal = displayedVal;
          displayedVal = latestBusVal;

          float diff = (float)displayedVal - (float)previousVal;

          // Plausibilitäts- & 70%-Sprungberechnung (A-11, A-12)
          if (previousVal >= 10) {
            deviationPercent = (diff / (float)previousVal) * 100.0f;
            isAlarmActive = (abs(deviationPercent) >= ALARM_DEV_THRESHOLD_PCT);
          } else {
            // Nullpunktsschutz gegen Division durch 0 (Schwellenwert >= 50 Digits)
            deviationPercent = (diff / 10.0f) * 100.0f;
            isAlarmActive = (abs(diff) >= ALARM_MIN_ABS_DIFF);
          }
        }

        renderDisplay();

        #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
          DEBUG_SERIAL.print(F("[HOLD] Neuer Wert: "));
          DEBUG_SERIAL.print(displayedVal);
          DEBUG_SERIAL.print(F(" | Dev: "));
          DEBUG_SERIAL.print(deviationPercent);
          DEBUG_SERIAL.print(F("% | Alarm: "));
          DEBUG_SERIAL.println(isAlarmActive ? F("JA") : F("NEIN"));
        #endif
      }
    }
  }
  lastButtonState = reading;
}
