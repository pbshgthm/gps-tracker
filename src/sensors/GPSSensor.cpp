// GPSSensor.cpp
#include "GPSSensor.h"

GPSSensor::GPSSensor() : gpsSerial(2)
{
}

bool GPSSensor::init()
{
  gpsSerial.begin(BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS - Initialization: OK");
  return true;
}

bool GPSSensor::read()
{
  while (gpsSerial.available() > 0)
  {
    gps.encode(gpsSerial.read());
  };

  if (gps.location.isValid() && gps.location.isUpdated())
  {
    data.location.isValid = true;
    data.location.latitude = gps.location.lat();
    data.location.longitude = gps.location.lng();
    data.location.age = gps.location.age();
  }
  else
  {
    data.location.isValid = false;
  }
  if (gps.altitude.isValid() && gps.altitude.isUpdated())
  {
    data.altitude.isValid = true;
    data.altitude.value = gps.altitude.meters();
    data.altitude.age = gps.altitude.age();
  }
  else
  {
    data.altitude.isValid = false;
  }

  if (gps.satellites.isValid() && gps.satellites.isUpdated())
  {
    data.satellites.isValid = true;
    data.satellites.value = gps.satellites.value();
    data.satellites.age = gps.satellites.age();
  }
  else
  {
    data.satellites.isValid = false;
  }
  if (gps.hdop.isValid() && gps.hdop.isUpdated())
  {
    data.hdop.isValid = true;
    data.hdop.value = gps.hdop.hdop();
    data.hdop.age = gps.hdop.age();
  }
  else
  {
    data.hdop.isValid = false;
  }
  if (gps.time.isValid() && gps.time.isUpdated())
  {
    data.time.isValid = true;
    data.time.value = gps.time.value();
    data.time.age = gps.time.age();
  }
  else
  {
    data.time.isValid = false;
  }

  if (gps.date.isValid() && gps.date.isUpdated())
  {
    data.date.isValid = true;
    data.date.value = gps.date.value();
    data.date.age = gps.date.age();
  }
  else
  {
    data.date.isValid = false;
  }
  return true;
}

void GPSSensor::print()
{
  Serial.println("----- GPS Data -----");

  // Print Location Data
  if (data.location.isValid)
  {
    Serial.printf("Location: Lat: %.6f, Lon: %.6f | Age: %u ms\n",
                  data.location.latitude, data.location.longitude, data.location.age);
  }
  else
  {
    Serial.println("Location: Invalid");
  }

  // Print Altitude Data
  if (data.altitude.isValid)
  {
    Serial.printf("Altitude: %.2f m | Age: %u ms\n",
                  data.altitude.value, data.altitude.age);
  }
  else
  {
    Serial.println("Altitude: Invalid");
  }

  // Print Date Data
  if (data.date.isValid)
  {
    Serial.printf("Date: %u | Age: %u ms\n",
                  data.date.value, data.date.age);
  }
  else
  {
    Serial.println("Date: Invalid");
  }

  // Print Time Data
  if (data.time.isValid)
  {
    Serial.printf("Time: %u | Age: %u ms\n",
                  data.time.value, data.time.age);
  }
  else
  {
    Serial.println("Time: Invalid");
  }

  // Print Satellites Data
  if (data.satellites.isValid)
  {
    Serial.printf("Satellites: %u | Age: %u ms\n",
                  data.satellites.value, data.satellites.age);
  }
  else
  {
    Serial.println("Satellites: Invalid");
  }

  // Print HDOP Data
  if (data.hdop.isValid)
  {
    Serial.printf("HDOP: %.2f | Age: %u ms\n",
                  data.hdop.value, data.hdop.age);
  }
  else
  {
    Serial.println("HDOP: Invalid");
  }

  Serial.flush();
}

GPSSensor::Data GPSSensor::get()
{
  return data;
}

void GPSSensor::powerOn()
{
  uint8_t ubxWakeup[] = {0x00};
  gpsSerial.write(ubxWakeup, sizeof(ubxWakeup));
}

void GPSSensor::powerOff()
{

  uint8_t ubxRxmPmreq[] = {
      0xB5, 0x62,             // UBX Header
      0x02, 0x41,             // Class = RXM, ID = PMREQ
      0x10, 0x00,             // Payload length = 16 bytes
      0x00,                   // version = 0
      0x00, 0x00, 0x00,       // reserved1
      0x00, 0x00, 0x00, 0x00, // duration = 0 (infinite)
      0x02, 0x00, 0x00, 0x00, // flags = 0x00000002 (backup mode)
      0x08, 0x00, 0x00, 0x00, // wakeupSources = 0x08 (uartrx only)
      0x5D, 0x4B              // Checksum (calculated manually)
  };

  gpsSerial.write(ubxRxmPmreq, sizeof(ubxRxmPmreq));
  gpsSerial.flush();
}