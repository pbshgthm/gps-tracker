#include <Arduino.h>
#include <SD.h>
#include "../environmental/EnvironmentalSensor.h"
#include "../accelerometer/AccelerometerSensor.h"
#include "../gps/GPSSensor.h"

const int SD_CS_PIN = 5; // Chip Select pin for SD card

RTC_DATA_ATTR static char currentFileName[32] = {0}; // Current log file name storage

class Storage
{
public:
  void init();
  void setLogFile();
  void write(
      const time_t &timestamp,
      const EnvironmentalSensor::Data &envData,
      const AccelerometerSensor::Data &accelData,
      const GPSSensor::Data &gpsData);
};
