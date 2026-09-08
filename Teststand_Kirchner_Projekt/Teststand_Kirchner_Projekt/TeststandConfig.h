#ifndef TESTSTAND_CONFIG_H
#define TESTSTAND_CONFIG_H

/**
 * @file TeststandConfig.h
 * @brief Zentrale Konfigurationsparameter fuer den 2-Arduino-Teststand (Arduino UNO R4)
 * @details Werner-von-Siemens-Schule Hildesheim - Fachschule Technik
 *          Mikrocontroller-Teststand (Lastenheft VDI 2206 / VDI 2221)
 */

#include <Arduino.h>

// =============================================================================
// 1. HARDWARE-PINBELEGUNG (Arduino UNO R4 Minima / WiFi)
// =============================================================================

// Sender-Arduino (Datenerfassung)
#define PIN_ANALOG_IN           A0      ///< Primaerer Analogeingang (Poti / 0..5V)
// Auf dem UNO R4 wird fuer den Inter-Arduino-Bus die Hardware-Schnittstelle Serial1 (Pin 0/1) genutzt
#define BUS_SERIAL_PORT         Serial1 ///< Hardware-UART fuer Arduino-zu-Arduino Kommunikation
#define DEBUG_SERIAL_PORT       Serial  ///< USB-Schnittstelle fuer PC-Monitor & Plotter

// Empfaenger-Arduino (Auswertung & HMI)
#define PIN_BUTTON_HOLD         4       ///< Taster fuer Hold-Aktualisierung (Active LOW, INPUT_PULLUP)
#define PIN_I2C_SDA             A4      ///< I2C Datenleitung zum LCD (alternativ dedizierter SDA-Pin)
#define PIN_I2C_SCL             A5      ///< I2C Taktleitung zum LCD (alternativ dedizierter SCL-Pin)

// =============================================================================
// 2. MODULARE PARAMETER (Anpassbar je nach Sensor / Testaufbau)
// =============================================================================

// Signalfilterung & Erfassung (Sender)
#define FILTER_WINDOW_SIZE      10      ///< Samples fuer Gleitenden Mittelwert (1..50)
#define SAMPLE_INTERVAL_MS      50      ///< Abtastzyklus ADC in ms (20 Hz)

// Bus-Kommunikation (UART via Serial1)
#define UART_BAUDRATE           9600    ///< Serielle Baudrate (9600, 19200, 115200)
#define BUS_TX_INTERVAL_MS      100     ///< Sendeintervall in ms (>= 10 Hz)
#define BUS_TIMEOUT_MS          2000    ///< Timeout fuer Verbindungsverlust in ms (> 2.0 s)

// Logik & Plausibilitaets-Grenzwerte (Empfaenger)
#define ALARM_DEV_THRESHOLD_PCT 70.0f   ///< Relative Sprung-Alarmschwelle in Prozent
#define ALARM_MIN_ABS_DIFF      50      ///< Mindest-Absolutdifferenz bei Werten nahe 0V (Digits)
#define DEBOUNCE_TIME_MS        50      ///< Software-Entprellzeit Taster in ms

// HMI / LC-Display (Empfaenger)
#define LCD_I2C_ADDR            0x27    ///< I2C-Adresse PCF8574 (0x27 oder 0x3F)
#define LCD_COLS                20      ///< Anzahl Spalten
#define LCD_ROWS                4       ///< Anzahl Zeilen

#endif // TESTSTAND_CONFIG_H
