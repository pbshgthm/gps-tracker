#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "sensors/EnvironmentalSensor.h"
#include "sensors/AccelerometerSensor.h"
#include "sensors/MagnetometerSensor.h"
#include "sensors/GPSSensor.h"
#include "display/Display.h"

// Pin Definitions for hardware connections
const int SDA_PIN = 21;    // I2C data line
const int SCL_PIN = 22;    // I2C clock line
const int LED_BUILTIN = 2; // Built-in LED pin for ESP32
const int BUTTON_PIN = 15; // Button pin for wake-up
const int SD_CS_PIN = 5;   // Chip Select pin for SD card

// Time definitions for sleep and display behavior
const uint64_t SLEEP_TIME_US = 10 * 1000000; // Deep sleep duration: 10 seconds (in microseconds)
const uint64_t DISPLAY_TIME_MS = 5000;       // Display on duration: 5 seconds (in milliseconds)

// Global sensor objects for collecting environmental data
EnvironmentalSensor envSensor;
AccelerometerSensor accelSensor;
MagnetometerSensor magSensor;
GPSSensor gpsSensor;
Display display;

// RTC memory variables - these persist across deep sleep cycles
RTC_DATA_ATTR static uint32_t bootIdentifier = 0;    // Unique identifier for each power-on boot
RTC_DATA_ATTR static char currentFileName[32] = {0}; // Current log file name storage

time_t now;

/*
 * File Management Functions
 */

String generateLogFileName()
{
  // Creates a unique filename by incrementing a counter until an unused name is found
  int fileCounter = 0;
  String fileName;

  do
  {
    fileName = String("/log_") + String(fileCounter) + String(".csv");
    fileCounter++;
  } while (SD.exists(fileName.c_str()));

  // Store the filename in RTC memory for persistence during sleep cycles
  fileName.toCharArray(currentFileName, sizeof(currentFileName));

  return fileName;
}

bool initializeSDCard()
{
  // Begin SD card communication using the specified chip select pin
  if (!SD.begin(SD_CS_PIN))
  {
    Serial.println("SD Card initialization failed!");
    return false;
  }

  Serial.println("SD Card initialization successful.");

  // Generate a new random identifier for fresh boots
  uint32_t newBootIdentifier = esp_random();

  // Check if this is a new boot by verifying boot identifier and filename
  bool isNewBoot = (bootIdentifier == 0 || strlen(currentFileName) == 0);

  if (isNewBoot)
  {
    // This is a fresh boot - set up new logging session
    bootIdentifier = newBootIdentifier;
    String newFileName = generateLogFileName();

    // Create new log file with headers
    File dataFile = SD.open(currentFileName, FILE_WRITE);
    if (dataFile)
    {
      dataFile.println("Time_ms,Temperature_C,Humidity_Percent,Pressure_hPa");
      dataFile.close();
      Serial.println("Created new log file: " + String(currentFileName));
    }
    else
    {
      Serial.println("Error creating new log file!");
      return false;
    }
  }
  else
  {
    // Continuing existing session after sleep
    Serial.println("Continuing with existing log file: " + String(currentFileName));

    // Verify the existing file is still accessible
    if (!SD.exists(currentFileName))
    {
      Serial.println("Warning: Existing log file not found! Creating new file.");
      String newFileName = generateLogFileName();
      File dataFile = SD.open(currentFileName, FILE_WRITE);
      if (dataFile)
      {
        dataFile.println("Time_ms,Temperature_C,Humidity_Percent,Pressure_hPa");
        dataFile.close();
      }
      else
      {
        return false;
      }
    }
  }

  return true;
}

/*
 * Sensor Management Functions
 */

void initializeSensors()
{
  // Initialize I2C communication bus
  Wire.begin(SDA_PIN, SCL_PIN);

  // Add delay for I2C bus stabilization
  delay(100);

  // Initialize environmental sensor and check for errors
  if (!envSensor.initialize())
  {
    Serial.println("Environmental sensor error: " + envSensor.getLastError());
  }
}

void logSensorData(const EnvironmentalSensor::Data &envData)
{
  // Verify sensor data validity before logging
  if (!envData.valid)
  {
    Serial.println("Invalid sensor data - skipping logging");
    return;
  }

  // Open existing log file in append mode
  File dataFile = SD.open(currentFileName, FILE_APPEND);
  if (dataFile)
  {

    // Create data string with all sensor readings
    String dataString = String(now) + "," +
                        String(envData.temperature, 2) + "," +
                        String(envData.humidity, 1) + "," +
                        String(envData.pressure, 1);

    // Write to file and close
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("Data logged: " + dataString);
  }
  else
  {
    Serial.println("Error opening log file for writing!");
  }
}

void readAndPrintSensors()
{
  // Read sensor data
  const bool envRead = envSensor.read();

  if (envRead)
  {
    // Display readings on serial monitor
    envSensor.print();

    // Log data to SD card
    logSensorData(envSensor.getData());
  }

  Serial.println("\n==============================\n");
}

/*
 * Power Management Functions
 */

void goToSleep()
{
  Serial.println("Going to sleep... Wake on timer or button press");
  Serial.flush(); // Ensure all serial data is transmitted

  // Power down peripherals
  digitalWrite(LED_BUILTIN, LOW);
  display.turnOff();

  // Configure wake-up sources
  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US); // Timer-based wake-up
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0); // Button-based wake-up (active LOW)

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

/*
 * Wake-up Handler Functions
 */

void handleButtonWakeup()
{
  Serial.println("Woken up by button press!");

  // Initialize hardware
  initializeSensors();

  // Initialize display
  if (!display.initialize())
  {
    Serial.println("Failed to initialize display!");
  }

  // Collect and log sensor data
  readAndPrintSensors();

  // Update display with sensor readings
  display.updateDisplay(
      envSensor.getData(),
      accelSensor.getData(),
      magSensor.getData(),
      gpsSensor.getData());

  // Keep display on for viewing time
  delay(DISPLAY_TIME_MS);
}

void handleTimerWakeup()
{
  Serial.println("Woken up by timer!");

  // Initialize hardware
  initializeSensors();

  // Collect and log sensor data
  readAndPrintSensors();
}

/*
 * Main Arduino Functions
 */

void setup()
{
  time(&now);
  // Initialize serial communication for debugging
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }

  // Configure GPIO pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SD_CS_PIN, OUTPUT);

  // Turn on LED to indicate active state
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize SD card logging system
  if (!initializeSDCard())
  {
    Serial.println("WARNING: SD card initialization failed - continuing without logging");
  }

  // Determine wake-up source and handle accordingly
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    // Wake-up triggered by button press
    handleButtonWakeup();
    break;

  case ESP_SLEEP_WAKEUP_TIMER:
    // Wake-up triggered by timer
    handleTimerWakeup();
    break;

  default:
    // First boot or unexpected wake-up
    Serial.println("First boot or unexpected wake-up");
    handleTimerWakeup();
    break;
  }

  // Enter sleep mode after handling wake-up
  goToSleep();
}

void loop()
{
  // This function will never be reached because the device
  // enters deep sleep at the end of setup()
}
