#include "EnvironmentalSensor.h"

bool EnvironmentalSensor::init()
{
  bme.begin(BME280_ADDR);

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
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
  bme.takeForcedMeasurement();

  data.temperature = bme.readTemperature();
  data.pressure = bme.readPressure() / 100.0F;
  data.humidity = bme.readHumidity();
  data.altitude = bme.readAltitude(1013.25);
  data.isValid = true;
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