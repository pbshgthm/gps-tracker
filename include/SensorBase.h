#pragma once
#include <Arduino.h>
#include <Wire.h>

class Sensor
{
protected:
  // Basic sensor state tracking
  unsigned long lastReadTime = 0;
  bool isInitialized = false;
  String lastError = "";
  uint8_t deviceAddress = 0;

  // Validation result structure to hold both status and error message
  struct ValidationResult
  {
    bool isValid;
    String error;

    ValidationResult(bool valid, const String &errorMsg = "")
        : isValid(valid), error(errorMsg) {}
  };

  // Protected validation method for derived classes to use
  ValidationResult validateI2CSensor()
  {
    // First check initialization status
    if (!isInitialized)
    {
      return ValidationResult(false, "Sensor not initialized");
    }

    // Then check connection if device uses I2C
    if (deviceAddress != 0)
    {
      if (!isConnected())
      {
        isInitialized = false; // Force reinitialization if connection lost
        return ValidationResult(false, "Device disconnected");
      }
    }

    return ValidationResult(true);
  }

public:
  virtual bool initialize() = 0;
  virtual bool read() = 0;
  virtual void print() = 0;

  virtual bool isConnected()
  {
    if (deviceAddress == 0)
      return true;
    Wire.beginTransmission(deviceAddress);
    return (Wire.endTransmission() == 0);
  }

  void printConnectionStatus(const char *sensorName)
  {
    if (deviceAddress != 0)
    {
      Serial.printf("%s (0x%02X) - Connection: %s\n",
                    sensorName,
                    deviceAddress,
                    isConnected() ? "OK" : "FAILED");
    }
    else
    {
      Serial.printf("%s - Connection: %s\n",
                    sensorName,
                    isConnected() ? "OK" : "FAILED");
    }
  }

  String getLastError() { return lastError; }
  unsigned long getLastReadTime() { return lastReadTime; }
  bool getInitStatus() { return isInitialized; }
};