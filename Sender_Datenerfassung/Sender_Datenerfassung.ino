/**
 * @file Sender_Datenerfassung.ino
 * @brief Datenerfassungs-Knoten fuer 2-Arduino-Teststand (Arduino UNO R4)
 * @details Werner-von-Siemens-Schule Hildesheim - Fachschule Technik
 * 
 * Funktionen:
 * - Kontinuierliche 10-Bit-ADC-Erfassung an Pin A0 (0..5V / 0..1023 Digits)
 * - Gleitender arithmetischer Mittelwert ueber N = 10 Samples (Abtastung: 20 Hz / 50 ms)
 * - Zyklische UART-Busuebertragung via Hardware-Serial1 (Pin 1 TX, >= 10 Hz / 100 ms)
 * - Diagnoseausgabe ueber USB-Serial fuer den Arduino Serial Plotter
 */

#include "TeststandConfig.h"

// =============================================================================
// GLOBALE VARIABLEN & PUFFER
// =============================================================================

// Ringpuffer fuer Gleitenden Mittelwert
uint16_t sampleBuffer[FILTER_WINDOW_SIZE];
uint8_t  bufferIndex = 0;
uint32_t bufferSum   = 0;

// Aktuelle Messwerte
uint16_t currentRawValue = 0;
uint16_t currentAvgValue = 0;

// Nicht-blockierende Timer-Zeitstempel
unsigned long lastSampleTime = 0;
unsigned long lastBusTxTime  = 0;

// =============================================================================
// SETUP ROUTINE
// =============================================================================
void setup() {
    // 1. USB-Diagnoseschnittstelle initialisieren
    DEBUG_SERIAL_PORT.begin(115200);
    
    // 2. Inter-Arduino-Bus (Hardware UART Serial1 an Pin 0/1) initialisieren
    BUS_SERIAL_PORT.begin(UART_BAUDRATE);

    // 3. Analogeingang konfigurieren
    pinMode(PIN_ANALOG_IN, INPUT);

    // 4. Ringpuffer vorbefuellen (verhindert Einschwingfehler beim Start)
    bufferSum = 0;
    for (uint8_t i = 0; i < FILTER_WINDOW_SIZE; i++) {
        uint16_t initialRead = analogRead(PIN_ANALOG_IN);
        sampleBuffer[i] = initialRead;
        bufferSum += initialRead;
        delay(5); // Kurze Einschwingzeit fuer ADC
    }
    bufferIndex = 0;
    currentRawValue = sampleBuffer[FILTER_WINDOW_SIZE - 1];
    currentAvgValue = (uint16_t)(bufferSum / FILTER_WINDOW_SIZE);

    DEBUG_SERIAL_PORT.println(F("=================================================="));
    DEBUG_SERIAL_PORT.println(F("SENDER-NODE (Datenerfassung UNO R4) INITIALISIERT"));
    DEBUG_SERIAL_PORT.print(F("Filtertiefe N: ")); DEBUG_SERIAL_PORT.println(FILTER_WINDOW_SIZE);
    DEBUG_SERIAL_PORT.print(F("Abtastzyklus: ")); DEBUG_SERIAL_PORT.print(SAMPLE_INTERVAL_MS); DEBUG_SERIAL_PORT.println(F(" ms"));
    DEBUG_SERIAL_PORT.print(F("Bus-Senderate: ")); DEBUG_SERIAL_PORT.print(BUS_TX_INTERVAL_MS); DEBUG_SERIAL_PORT.println(F(" ms"));
    DEBUG_SERIAL_PORT.println(F("=================================================="));
}

// =============================================================================
// HAUPTSCHLEIFE (LOOP)
// =============================================================================
void loop() {
    unsigned long currentMillis = millis();

    // -------------------------------------------------------------------------
    // 1. ANALOGE MESSWERTERFASSUNG & FILTERUNG (alle 50 ms / 20 Hz)
    // -------------------------------------------------------------------------
    if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentMillis;

        // Rohwert einlesen (10-Bit: 0 .. 1023)
        currentRawValue = analogRead(PIN_ANALOG_IN);

        // Gleitenden Mittelwert ueber FIFO-Ringpuffer aktualisieren
        bufferSum -= sampleBuffer[bufferIndex];
        sampleBuffer[bufferIndex] = currentRawValue;
        bufferSum += sampleBuffer[bufferIndex];

        bufferIndex = (bufferIndex + 1) % FILTER_WINDOW_SIZE;
        currentAvgValue = (uint16_t)(bufferSum / FILTER_WINDOW_SIZE);
    }

    // -------------------------------------------------------------------------
    // 2. ZYKLISCHE UART-BUSUEBERTRAGUNG (alle 100 ms / 10 Hz)
    // -------------------------------------------------------------------------
    if (currentMillis - lastBusTxTime >= BUS_TX_INTERVAL_MS) {
        lastBusTxTime = currentMillis;

        // Telegramm-Format: <VAL:xxxx;RAW:xxxx>\n
        char telegram[32];
        snprintf(telegram, sizeof(telegram), "<VAL:%04u;RAW:%04u>\n", currentAvgValue, currentRawValue);

        // An Empfaenger-Arduino senden (via Hardware-UART Serial1)
        BUS_SERIAL_PORT.print(telegram);

        // Diagnose-Ausgabe fuer Serial Monitor & Serial Plotter
        DEBUG_SERIAL_PORT.print(F("RAW:"));
        DEBUG_SERIAL_PORT.print(currentRawValue);
        DEBUG_SERIAL_PORT.print(F("\tAVG:"));
        DEBUG_SERIAL_PORT.print(currentAvgValue);
        DEBUG_SERIAL_PORT.print(F("\tFRAME:"));
        DEBUG_SERIAL_PORT.print(telegram);
    }
}
