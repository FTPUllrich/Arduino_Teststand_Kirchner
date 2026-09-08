/**
 * @file Sender.ino
 * @brief Sender-Knoten: Analoge Messwerterfassung, FIFO-Ringpuffer-Glättung & UART-Übertragung
 * @details Werner-von-Siemens-Schule Hildesheim - 2x Arduino Mikrocontroller-Teststand
 *          Kompatibel mit Arduino UNO R4 (Minima/WiFi) & Arduino UNO R3
 */

#include "TeststandConfig.h"

// Hardware-UART Zuweisung je nach Board
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  #define BUS_SERIAL    Serial1  // Hardware-UART an Pin 0 (RX) & Pin 1 (TX)
  #define DEBUG_SERIAL  Serial   // USB-Schnittstelle zum PC
#else
  #define BUS_SERIAL    Serial   // UNO R3 Standard-Serial an Pin 0/1
  #define DEBUG_SERIAL  Serial
#endif

// Ringpuffer für gleitenden Mittelwert
uint16_t readings[FILTER_WINDOW_SIZE];
uint8_t readIndex = 0;
uint32_t ringTotal = 0;
uint16_t filteredAverage = 0;
uint16_t latestRawValue = 0;

// Zeitsteuerung (nicht-blockierend via millis)
unsigned long lastSampleTime = 0;
unsigned long lastTxTime = 0;

void setup() {
  BUS_SERIAL.begin(UART_BAUDRATE);
  
  #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
    DEBUG_SERIAL.begin(115200);
    DEBUG_SERIAL.println(F("[SENDER] Arduino UNO R4 Datenerfassung initialisiert."));
  #endif

  // Initiales Füllen des Ringpuffers (Kaltstart-Einschwingen verhindern)
  latestRawValue = analogRead(PIN_ANALOG_IN);
  for (uint8_t i = 0; i < FILTER_WINDOW_SIZE; i++) {
    readings[i] = latestRawValue;
    ringTotal += readings[i];
  }
  filteredAverage = ringTotal / FILTER_WINDOW_SIZE;
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Abtastung & Gleitender Mittelwert (A-01, A-02: 20 Hz / 50ms)
  if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = currentMillis;

    latestRawValue = analogRead(PIN_ANALOG_IN);
    
    // FIFO-Ringpuffer aktualisieren
    ringTotal -= readings[readIndex];
    readings[readIndex] = latestRawValue;
    ringTotal += readings[readIndex];
    readIndex = (readIndex + 1) % FILTER_WINDOW_SIZE;

    filteredAverage = (uint16_t)(ringTotal / FILTER_WINDOW_SIZE);
  }

  // 2. Zyklische Busübertragung via UART (A-04: >= 10 Hz / 100ms)
  if (currentMillis - lastTxTime >= BUS_TX_INTERVAL_MS) {
    lastTxTime = currentMillis;

    // Telegramm-Format: <VAL:xxxx;RAW:xxxx>\n
    char txBuffer[32];
    snprintf(txBuffer, sizeof(txBuffer), "<VAL:%04u;RAW:%04u>", filteredAverage, latestRawValue);
    BUS_SERIAL.println(txBuffer);

    #if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
      // Paralleles Debugging am PC Serial Monitor / Plotter
      DEBUG_SERIAL.print(F("Raw:"));
      DEBUG_SERIAL.print(latestRawValue);
      DEBUG_SERIAL.print(F("\tAvg:"));
      DEBUG_SERIAL.println(filteredAverage);
    #endif
  }
}
