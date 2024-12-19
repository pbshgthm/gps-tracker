// display/Display.cpp
#include "Display.h"

Display::Display() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

void Display::init()
{
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.clearBuffer();
  Serial.println("Display - Initialization: OK");
  u8g2.setPowerSave(1);
}

void Display::turnOn()
{
  Serial.println("Display on");
  u8g2.setPowerSave(0);
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void Display::turnOff()
{
  Serial.println("Display off");
  u8g2.setPowerSave(1); // Put display in power-saving mode
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void Display::updateDisplay(
    const EnvironmentalSensor::Data &envData,
    const AccelerometerSensor::Data &accelData,
    const GPSSensor::Data &gpsData)
{
  char buf[32];
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_8x13_tf);

  // Line 1: Environmental Sensor Data
  snprintf(buf, sizeof(buf), "%.1fC %.0f%%H %.0fmb",
           envData.temperature, envData.humidity, envData.pressure);
  u8g2.drawStr(0, 13, buf); // First line

  // Line 2: Accelerometer Data
  snprintf(buf, sizeof(buf), "Accel: %.2f m/s2", accelData.magnitude);
  u8g2.drawStr(0, 26, buf); // Second line

  // Line 3: GPS Location
  if (gpsData.location.isValid)
  {
    snprintf(buf, sizeof(buf), "GPS: %.5f,%.5f",
             gpsData.location.latitude, gpsData.location.longitude);
  }
  else
  {
    snprintf(buf, sizeof(buf), "GPS: Invalid");
  }
  u8g2.drawStr(0, 39, buf); // Third line

  // Line 4: Altitude, Satellites, and Time
  if (gpsData.altitude.isValid && gpsData.satellites.isValid)
  {
    snprintf(buf, sizeof(buf), "Alt: %.0fm Sats:%d",
             gpsData.altitude.value, gpsData.satellites.value);
  }
  else
  {
    snprintf(buf, sizeof(buf), "Alt: N/A Sats: N/A");
  }
  u8g2.drawStr(0, 52, buf); // Fourth line

  // Line 5: Time
  if (gpsData.time.isValid)
  {
    uint32_t timeValue = gpsData.time.value; // HHMMSS format
    snprintf(buf, sizeof(buf), "Time: %02lu:%02lu:%02lu",
             (timeValue / 10000) % 100, // Hours
             (timeValue / 100) % 100,   // Minutes
             timeValue % 100);          // Seconds
  }
  else
  {
    snprintf(buf, sizeof(buf), "Time: N/A");
  }
  u8g2.drawStr(0, 64, buf); // Fifth line

  u8g2.sendBuffer();
}
