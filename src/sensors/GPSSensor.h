// GPSSensor.h
#pragma once
#include "SensorBase.h"
#include <TinyGPS++.h>

class GPSSensor : public Sensor
{
public:
  struct Data
  {
    bool valid;
    double latitude;
    double longitude;
    double altitude;
    int satellites; // Changed from int to String
    double speed;
    double course;
    float hdop;
    unsigned long age;
  };

  bool initialize() override;
  bool read() override;
  void print() override;
  Data getData();

private:
  TinyGPSPlus gps;
  Data data;
  static const int GPS_RX_PIN = 16;
  static const int GPS_TX_PIN = 17;
};