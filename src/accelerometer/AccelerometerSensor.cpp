#include "AccelerometerSensor.h"

bool AccelerometerSensor::init()
{

  accSensor.begin(ACCELEROMETER_ADDR);
  accSensor.setRange(LIS3DH_RANGE_2_G);
  Serial.println("LIS3DH - Initialization: OK");
  return true;
}

bool AccelerometerSensor::read()
{

  sensors_event_t event;
  accSensor.getEvent(&event);

  float prevX = data.x;
  float prevY = data.y;
  float prevZ = data.z;
  float prevMagnitude = data.magnitude;

  data.x = event.acceleration.x;
  data.y = event.acceleration.y;
  data.z = event.acceleration.z;
  data.magnitude = sqrt(pow(event.acceleration.x, 2) +
                        pow(event.acceleration.y, 2) +
                        pow(event.acceleration.z, 2));

  data.diffX = data.x - prevX;
  data.diffY = data.y - prevY;
  data.diffZ = data.z - prevZ;
  data.diffMagnitude = data.magnitude - prevMagnitude;

  return true;
}

void AccelerometerSensor::print()
{
  Serial.println("\n----- Accelerometer Data -----\n");
  Serial.printf("X: %.2f Y: %.2f Z: %.2f m/s²\n", data.x, data.y, data.z);
  Serial.printf("Magnitude: %.2f m/s²\n", data.magnitude);
  Serial.printf("Diff X: %.2f Y: %.2f Z: %.2f m/s²\n", data.diffX, data.diffY, data.diffZ);
  Serial.printf("Diff Magnitude: %.2f m/s²\n", data.diffMagnitude);
}

AccelerometerSensor::Data AccelerometerSensor::getData()
{
  return data;
}