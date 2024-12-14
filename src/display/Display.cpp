// // display/Display.cpp
// #include "Display.h"

// Display::Display() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

// bool Display::initialize()
// {

//   u8g2.begin();
//   Serial.println("Display on");
//   u8g2.setPowerSave(0);
//   return true;
// }

// void Display::turnOff()
// {
//   Serial.println("Display off");
//   u8g2.setPowerSave(1); // Put display in power-saving mode
//   u8g2.clearBuffer();
//   u8g2.sendBuffer();
// }

// void Display::updateDisplay(
//     const EnvironmentalSensor::Data &envData,
//     const AccelerometerSensor::Data &accelData,
//     const MagnetometerSensor::Data &magData,
//     const GPSSensor::Data &gpsData)
// {

//   char buf[32];
//   u8g2.clearBuffer();
//   u8g2.setFont(u8g2_font_8x13_tf);

//   // BME280 Data
//   snprintf(buf, sizeof(buf), "%.1fC %.0f%%H %.0fmb",
//            envData.temperature, envData.humidity, envData.pressure);
//   u8g2.drawStr(0, FONT_HEIGHT, buf);

//   // Accelerometer Data
//   snprintf(buf, sizeof(buf), "%.2f m/s2", accelData.magnitude);
//   u8g2.drawStr(0, FONT_HEIGHT + LINE_SPACING, buf);

//   // Compass Data
//   snprintf(buf, sizeof(buf), "%.1f %s", magData.heading, magData.direction.c_str());
//   u8g2.drawStr(0, FONT_HEIGHT + LINE_SPACING * 2, buf);

//   // // GPS Data
//   // if (gpsData.valid)
//   // {
//   //   snprintf(buf, sizeof(buf), "%.6f,%.6f %.0fm S%d",
//   //            gpsData.latitude, gpsData.longitude, gpsData.altitude, gpsData.satellites);
//   //   u8g2.drawStr(0, FONT_HEIGHT + LINE_SPACING * 3, buf);
//   // }
//   // else
//   // {
//   //   snprintf(buf, sizeof(buf), "No GPS S:%d", gpsData.satellites);
//   //   u8g2.drawStr(0, FONT_HEIGHT + LINE_SPACING * 3, buf);
//   // }

//   u8g2.sendBuffer();
// }