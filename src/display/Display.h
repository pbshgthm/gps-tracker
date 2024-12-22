// display/Display.h
#pragma once
#include <U8g2lib.h>
#include "../environmental/EnvironmentalSensor.h"
#include "../accelerometer/AccelerometerSensor.h"
#include "../gps/GPSSensor.h"

class Display
{
public:
  Display();
  void init();
  void turnOn();
  void turnOff();

  void greet();
  void showData(
      const EnvironmentalSensor::Data &envData,
      const AccelerometerSensor::Data &accelData,
      const GPSSensor::Data &gpsData);
  void alertWifi();
  void alertWaypoint();

private:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
  static const uint8_t FONT_HEIGHT = 13;
  static const uint8_t LINE_SPACING = 16;
  static const uint8_t PERCENT_SYMBOL_OFFSET = 0;
};