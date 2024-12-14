// #include <Arduino.h>
// #include <SD.h>
// #include "sensors/EnvironmentalSensor.h"

// const int SD_CS_PIN = 5; // Chip Select pin for SD card

// // RTC memory variables - these persist across deep sleep cycles
// RTC_DATA_ATTR static uint32_t bootIdentifier = 0;    // Unique identifier for each power-on boot
// RTC_DATA_ATTR static char currentFileName[32] = {0}; // Current log file name storage

// class Storage
// {
// public:
//   bool initializeSDCard();
//   void logSensorData(const EnvironmentalSensor::Data &data, const time_t &timestamp);
//   String generateLogFileName();
// };
