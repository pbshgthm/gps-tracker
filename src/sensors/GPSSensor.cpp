// GPSSensor.cpp
#include "GPSSensor.h"

bool GPSSensor::initialize()
{
  printConnectionStatus("GPS NEO-7M");
  Serial1.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  isInitialized = true;
  Serial.println("GPS NEO-7M - Initialization: OK");
  return true;
}

bool GPSSensor::read()
{
  if (!isInitialized)
  {
    lastError = "Sensor not initialized";
    return false;
  }

  while (Serial1.available() > 0)
  {
    if (gps.encode(Serial1.read()))
    {
      data.valid = gps.location.isValid();

      data.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;

      if (data.valid)
      {
        data.latitude = gps.location.lat();
        data.longitude = gps.location.lng();
        data.altitude = gps.altitude.meters();
        data.speed = gps.speed.kmph();
        data.course = gps.course.deg();
        data.hdop = gps.hdop.value();
        data.age = gps.location.age();
      }
      lastReadTime = millis();
      return true;
    }
  }
  return false;
}

void GPSSensor::print()
{
  Serial.println("\n=== GPS Data (NEO-7M) ===");
  Serial.printf("Satellites : %d\n", data.satellites);

  if (data.valid)
  {
    Serial.printf("Latitude: %.6f°\n", data.latitude);
    Serial.printf("Longitude: %.6f°\n", data.longitude);
    Serial.printf("Altitude: %.1f m\n", data.altitude);
    Serial.printf("Speed: %.1f km/h\n", data.speed);
    Serial.printf("Course: %.1f°\n", data.course);
    Serial.printf("HDOP: %.1f ", data.hdop);

    if (data.hdop < 1.0)
      Serial.println("(Ideal)");
    else if (data.hdop < 2.0)
      Serial.println("(Excellent)");
    else if (data.hdop < 5.0)
      Serial.println("(Good)");
    else if (data.hdop < 10.0)
      Serial.println("(Moderate)");
    else
      Serial.println("(Poor)");

    Serial.printf("Fix Age: %lu ms\n", data.age);
  }
  else
  {
    Serial.println("GPS signal not valid");
  }
}

GPSSensor::Data GPSSensor::getData()
{
  return data;
}