// #include "Storage.h"

// String Storage::generateLogFileName()
// {
//   // Creates a unique filename by incrementing a counter until an unused name is found
//   int fileCounter = 0;
//   String fileName;

//   do
//   {
//     fileName = String("/log_") + String(fileCounter) + String(".csv");
//     fileCounter++;
//   } while (SD.exists(fileName.c_str()));

//   // Store the filename in RTC memory for persistence during sleep cycles
//   fileName.toCharArray(currentFileName, sizeof(currentFileName));

//   return fileName;
// }

// bool Storage::initializeSDCard()
// {
//   // Begin SD card communication using the specified chip select pin
//   if (!SD.begin(SD_CS_PIN))
//   {
//     Serial.println("SD Card initialization failed!");
//     return false;
//   }

//   Serial.println("SD Card initialization successful.");

//   // Generate a new random identifier for fresh boots
//   uint32_t newBootIdentifier = esp_random();

//   // Check if this is a new boot by verifying boot identifier and filename
//   bool isNewBoot = (bootIdentifier == 0 || strlen(currentFileName) == 0);

//   if (isNewBoot)
//   {
//     // This is a fresh boot - set up new logging session
//     bootIdentifier = newBootIdentifier;
//     String newFileName = generateLogFileName();

//     // Create new log file with headers
//     File dataFile = SD.open(currentFileName, FILE_WRITE);
//     if (dataFile)
//     {
//       dataFile.println("Time_ms,Temperature_C,Humidity_Percent,Pressure_hPa");
//       dataFile.close();
//       Serial.println("Created new log file: " + String(currentFileName));
//     }
//     else
//     {
//       Serial.println("Error creating new log file!");
//       return false;
//     }
//   }
//   else
//   {
//     // Continuing existing session after sleep
//     Serial.println("Continuing with existing log file: " + String(currentFileName));

//     // Verify the existing file is still accessible
//     if (!SD.exists(currentFileName))
//     {
//       Serial.println("Warning: Existing log file not found! Creating new file.");
//       String newFileName = generateLogFileName();
//       File dataFile = SD.open(currentFileName, FILE_WRITE);
//       if (dataFile)
//       {
//         dataFile.println("Time_ms,Temperature_C,Humidity_Percent,Pressure_hPa");
//         dataFile.close();
//       }
//       else
//       {
//         return false;
//       }
//     }
//   }

//   return true;
// }

// void Storage::logSensorData(const EnvironmentalSensor::Data &envData, const time_t &now)
// {
//   // Verify sensor data validity before logging
//   if (!envData.valid)
//   {
//     Serial.println("Invalid sensor data - skipping logging");
//     return;
//   }

//   // Open existing log file in append mode
//   File dataFile = SD.open(currentFileName, FILE_APPEND);
//   if (dataFile)
//   {

//     // Create data string with all sensor readings
//     String dataString = String(now) + "," +
//                         String(envData.temperature, 2) + "," +
//                         String(envData.humidity, 1) + "," +
//                         String(envData.pressure, 1);

//     // Write to file and close
//     dataFile.println(dataString);
//     dataFile.close();
//     Serial.println("Data logged: " + dataString);
//   }
//   else
//   {
//     Serial.println("Error opening log file for writing!");
//   }
// }
