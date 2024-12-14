// GPSSensor.h
#pragma once
#include <TinyGPS++.h>
#include <HardwareSerial.h>

class GPSSensor
{
public:
  struct Data
  {
    struct Location
    {
      bool isValid;
      double latitude;
      double longitude;
      uint32_t age;
    } location;

    struct Altitude
    {
      bool isValid;
      double value;
      uint32_t age;
    } altitude;

    struct Date
    {
      bool isValid;
      uint32_t value;
      uint32_t age;
    } date;

    struct Time
    {
      bool isValid;
      uint32_t value;
      uint32_t age;
    } time;

    struct Satellites
    {
      bool isValid;
      uint32_t value;
      uint32_t age;
    } satellites;

    struct HDOP
    {
      bool isValid;
      float value;
      uint32_t age;
    } hdop;
  };

  GPSSensor();

  bool init();
  void powerOn();
  void powerOff();
  bool read();
  void print();
  Data getData();

private:
  TinyGPSPlus gps;
  Data data;
  HardwareSerial gpsSerial;
  static const int GPS_RX_PIN = 16;
  static const int GPS_TX_PIN = 17;
  static const int BAUD_RATE = 9600;
};