// display/Display.h
#pragma once
#include <U8g2lib.h>
#include "../sensors/EnvironmentalSensor.h"
#include "../sensors/AccelerometerSensor.h"
#include "../sensors/MagnetometerSensor.h"
#include "../sensors/GPSSensor.h"

class Display
{
public:
  Display();
  bool initialize();

  void turnOff();

  void updateDisplay(
      const EnvironmentalSensor::Data &envData,
      const AccelerometerSensor::Data &accelData,
      const MagnetometerSensor::Data &magData,
      const GPSSensor::Data &gpsData);

private:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
  static const uint8_t FONT_HEIGHT = 13;
  static const uint8_t LINE_SPACING = 16;
  static const uint8_t PERCENT_SYMBOL_OFFSET = 0;
};