#include "EnvironmentalSensor.h"

bool EnvironmentalSensor::init()
{
  envSensor.begin(BME280_ADDR);

  envSensor.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X8, // temperature
                        Adafruit_BME280::SAMPLING_X8, // pressure
                        Adafruit_BME280::SAMPLING_X8, // humidity
                        Adafruit_BME280::FILTER_OFF);

  Serial.println("BME280 - Initialization: OK");
  return true;
}

bool EnvironmentalSensor::read()
{
  // Start measurement in forced mode
  envSensor.takeForcedMeasurement();

  data.temperature = envSensor.readTemperature();
  data.pressure = envSensor.readPressure() / 100.0F;
  data.humidity = envSensor.readHumidity();
  data.altitude = envSensor.readAltitude(1013.25);
  return true;
}

void EnvironmentalSensor::print()
{
  Serial.println("\n----- Environmental Data -----");
  Serial.printf("Temperature: %.2f °C\n", data.temperature);
  Serial.printf("Pressure: %.1f mb\n", data.pressure);
  Serial.printf("Humidity: %.1f %%\n", data.humidity);
  Serial.printf("Altitude: %.1f m\n", data.altitude);
}

EnvironmentalSensor::Data EnvironmentalSensor::getData()
{
  return data;
}