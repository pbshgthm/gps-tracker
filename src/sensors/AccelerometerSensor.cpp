#include "AccelerometerSensor.h"

AccelerometerSensor::AccelerometerSensor()
{
  deviceAddress = ACCELEROMETER_ADDR;
}

bool AccelerometerSensor::initialize()
{
  printConnectionStatus("LIS3DH");

  if (!isConnected())
  {
    lastError = "LIS3DH not found";
    Serial.println("LIS3DH - Initialization: FAILED");
    data.valid = false;
    return false;
  }

  if (!lis.begin(deviceAddress))
  {
    lastError = "LIS3DH initialization failed";
    Serial.println("LIS3DH - Initialization: FAILED");
    data.valid = false;
    return false;
  }

  lis.setRange(LIS3DH_RANGE_2_G);
  isInitialized = true;
  data.valid = true;
  Serial.println("LIS3DH - Initialization: OK");
  return true;
}

bool AccelerometerSensor::read()
{
  // Validate sensor state before attempting to read
  ValidationResult validation = validateI2CSensor();
  if (!validation.isValid)
  {
    lastError = validation.error;
    data.valid = false;
    return false;
  }

  sensors_event_t event;
  lis.getEvent(&event);

  data.x = event.acceleration.x;
  data.y = event.acceleration.y;
  data.z = event.acceleration.z;

  data.magnitude = sqrt(pow(event.acceleration.x, 2) +
                        pow(event.acceleration.y, 2) +
                        pow(event.acceleration.z, 2));

  data.valid = true;
  lastReadTime = millis();
  return true;
}

void AccelerometerSensor::print()
{
  Serial.println("\n=== Accelerometer Data (LIS3DH) ===");
  Serial.printf("Magnitude: %.2f m/s²\n", data.magnitude);
  Serial.printf("Raw X: %.2f Y: %.2f Z: %.2f m/s²\n", data.x, data.y, data.z);
}

AccelerometerSensor::Data AccelerometerSensor::getData()
{
  return data;
}