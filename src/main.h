#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "sensors/EnvironmentalSensor.h"
#include "sensors/AccelerometerSensor.h"
#include "sensors/MagnetometerSensor.h"
#include "sensors/GPSSensor.h"
#include "display/Display.h"
#include "storage/Storage.h"

// Pin Definitions for hardware connections
const int SDA_PIN = 21;    // I2C data line
const int SCL_PIN = 22;    // I2C clock line
const int LED_BUILTIN = 2; // Built-in LED pin for ESP32
const int BUTTON_PIN = 15; // Button pin for wake-up

// Time definitions for sleep and display behavior
const uint64_t SLEEP_TIME_US = 10 * 1000000; // Deep sleep duration: 10 seconds (in microseconds)
const uint64_t DISPLAY_TIME_MS = 5000;       // Display on duration: 5 seconds (in milliseconds)

// Global sensor objects for collecting environmental data
EnvironmentalSensor envSensor;
AccelerometerSensor accelSensor;
MagnetometerSensor magSensor;
GPSSensor gpsSensor;
// Display display;

// Global storage object for logging sensor data
// Storage storage;

// Global time variable for timestamping sensor readings
time_t now;