#include "MagnetometerSensor.h"

MagnetometerSensor::MagnetometerSensor()
{
  deviceAddress = QMC5883L_ADDR;
}

bool MagnetometerSensor::initialize()
{
  printConnectionStatus("QMC5883L");

  if (!isConnected())
  {
    lastError = "QMC5883L not found";
    Serial.println("QMC5883L - Initialization: FAILED");
    return false;
  }

  compass.init();
  compass.setCalibration(-1309, 1729, -1669, 1575, -1563, 1597);
  isInitialized = true;
  Serial.println("QMC5883L - Initialization: OK");
  return true;
}

bool MagnetometerSensor::read()
{
  // Validate sensor state before attempting to read
  ValidationResult validation = validateI2CSensor();
  if (!validation.isValid)
  {
    lastError = validation.error;
    data.valid = false;
    return false;
  }

  data.heading = compass.getAzimuth();
  data.x = compass.getX();
  data.y = compass.getY();
  data.z = compass.getZ();
  data.direction = getCompassDirection(data.heading);

  lastReadTime = millis();
  return true;
}

void MagnetometerSensor::print()
{
  Serial.println("\n=== Magnetometer Data (QMC5883L) ===");
  Serial.printf("Heading: %.1f° (%s)\n", data.heading, data.direction.c_str());
  Serial.printf("X: %d\n", data.x);
  Serial.printf("Y: %d\n", data.y);
  Serial.printf("Z: %d\n", data.z);
}

String MagnetometerSensor::getCompassDirection(float heading)
{
  if (heading < 22.5)
    return "N";
  if (heading < 67.5)
    return "NE";
  if (heading < 112.5)
    return "E";
  if (heading < 157.5)
    return "SE";
  if (heading < 202.5)
    return "S";
  if (heading < 247.5)
    return "SW";
  if (heading < 292.5)
    return "W";
  if (heading < 337.5)
    return "NW";
  return "N";
}

MagnetometerSensor::Data MagnetometerSensor::getData()
{
  return data;
}