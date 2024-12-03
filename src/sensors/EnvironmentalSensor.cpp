#include "EnvironmentalSensor.h"

EnvironmentalSensor::EnvironmentalSensor()
{
  deviceAddress = BME280_ADDR;
}

bool EnvironmentalSensor::initialize()
{
  printConnectionStatus("BME280");

  if (!isConnected())
  {
    lastError = "BME280 not found";
    Serial.println("BME280 - Initialization: FAILED");
    return false;
  }

  if (!bme.begin(deviceAddress))
  {
    lastError = "BME280 initialization failed";
    Serial.println("BME280 - Initialization: FAILED");
    return false;
  }

  // Configure BME280 settings
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X8, // temperature
                  Adafruit_BME280::SAMPLING_X8, // pressure
                  Adafruit_BME280::SAMPLING_X8, // humidity
                  Adafruit_BME280::FILTER_OFF);

  isInitialized = true;
  Serial.println("BME280 - Initialization: OK");
  return true;
}

bool EnvironmentalSensor::read()
{
  // Validate sensor state before attempting to read
  ValidationResult validation = validateI2CSensor();
  if (!validation.isValid)
  {
    lastError = validation.error;
    data.valid = false;
    return false;
  }

  // Start measurement in forced mode
  bme.takeForcedMeasurement();

  data.temperature = bme.readTemperature();
  data.pressure = bme.readPressure() / 100.0F;
  data.humidity = bme.readHumidity();
  data.altitude = bme.readAltitude(1013.25);
  data.valid = true;
  lastReadTime = millis();
  return true;
}

void EnvironmentalSensor::print()
{
  Serial.println("\n=== Environmental Data (BME280) ===");
  Serial.printf("Temperature: %.2f °C\n", data.temperature);
  Serial.printf("Pressure: %.1f mb\n", data.pressure);
  Serial.printf("Humidity: %.1f %%\n", data.humidity);
  Serial.printf("Altitude: %.1f m\n", data.altitude);
}

EnvironmentalSensor::Data EnvironmentalSensor::getData()
{
  return data;
}