#include "Storage.h"

void formatCSV(char *buffer, size_t bufferSize,
               const time_t &timestamp,
               const EnvironmentalSensor::Data &envData,
               const AccelerometerSensor::Data &accelData,
               const GPSSensor::Data &gpsData)
{
  snprintf(buffer, bufferSize,
           "%ld,%.2f,%.1f,%.1f,%.2f,%.2f,%.2f,%.2f,%.5f,%.5f,%d,%.2f,%d,%lu,%d,%lu,%d,%d,%d,%.2f,%d",
           timestamp,
           envData.temperature,
           envData.humidity,
           envData.pressure,
           accelData.x,
           accelData.y,
           accelData.z,
           accelData.magnitude,
           gpsData.location.latitude,
           gpsData.location.longitude,
           gpsData.location.age,
           gpsData.altitude.value,
           gpsData.altitude.age,
           gpsData.date.value,
           gpsData.date.age,
           gpsData.time.value,
           gpsData.time.age,
           gpsData.satellites.value,
           gpsData.satellites.age,
           gpsData.hdop.value,
           gpsData.hdop.age);
}

void Storage::setLogFile()
{

  if (currentFileName[0] != 0 && SD.exists(currentFileName))
  {
    Serial.println("Current log file: " + String(currentFileName));
    return;
  }
  else
  {
    Serial.println("No current log file");
  }

  int fileCounter = 0;
  while (true)
  {
    snprintf(currentFileName, sizeof(currentFileName), "/log_%d.csv", fileCounter);
    if (!SD.exists(currentFileName))
      break;
    fileCounter++;
  }

  File dataFile = SD.open(currentFileName, FILE_WRITE);
  if (dataFile)
  {
    // Write the header row with meaningful terms
    dataFile.println(
        "Timestamp,"
        "Temperature (C),Pressure (hPa),Humidity (%),"
        "Accel X (m/s^2),Accel Y (m/s^2),Accel Z (m/s^2),Accel Magnitude (m/s^2),"
        "Latitude (deg),Longitude (deg),Location Age (ms),"
        "Altitude (m),Altitude Age (ms),"
        "Date (YYYYMMDD),Date Age (ms),"
        "Time (HHMMSS),Time Age (ms),"
        "Satellites Count,Satellites Age (ms),"
        "HDOP,HDOP Age (ms)");
    dataFile.close();
    Serial.println("Created new log file: " + String(currentFileName));
  }
  else
  {
    Serial.println("Error creating new log file!");
  }
}

void Storage::init()
{
  if (!SD.begin(SD_CS_PIN))
  {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialization successful.");
  setLogFile();
}

void Storage::write(
    const time_t &timestamp,
    const EnvironmentalSensor::Data &envData,
    const AccelerometerSensor::Data &accelData,
    const GPSSensor::Data &gpsData)
{
  if (currentFileName[0] == 0)
  {
    Serial.println("No valid log file set!");
    return;
  }

  File file = SD.open(currentFileName, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  char dataString[512];
  formatCSV(dataString, sizeof(dataString), timestamp, envData, accelData, gpsData);

  file.println(dataString);
  file.close();
}