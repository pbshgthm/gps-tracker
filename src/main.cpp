#include "main.h"

/*
 * Sensor Management Functions
 */

void initializeSensors()
{
  // Initialize I2C communication bus
  Wire.begin(SDA_PIN, SCL_PIN);

  // Add delay for I2C bus stabilization
  delay(100);

  // Initialize environmental sensor and check for errors
  if (!envSensor.initialize())
  {
    Serial.println("Environmental sensor error: " + envSensor.getLastError());
  }
}

void readAndPrintSensors()
{
  // Read sensor data
  const bool envRead = envSensor.read();

  if (envRead)
  {
    // Display readings on serial monitor
    envSensor.print();

    // Log data to SD card
    storage.logSensorData(envSensor.getData(), now);
  }

  Serial.println("\n==============================\n");
}

/*
 * Power Management Functions
 */

void goToSleep()
{
  Serial.println("Going to sleep... Wake on timer or button press");
  Serial.flush(); // Ensure all serial data is transmitted

  // Power down peripherals
  digitalWrite(LED_BUILTIN, LOW);
  display.turnOff();

  // Configure wake-up sources
  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US); // Timer-based wake-up
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0); // Button-based wake-up (active LOW)

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

/*
 * Wake-up Handler Functions
 */

void handleButtonWakeup()
{
  Serial.println("Woken up by button press!");

  // Initialize hardware
  initializeSensors();

  // Initialize display
  if (!display.initialize())
  {
    Serial.println("Failed to initialize display!");
  }

  // Collect and log sensor data
  readAndPrintSensors();

  // Update display with sensor readings
  display.updateDisplay(
      envSensor.getData(),
      accelSensor.getData(),
      magSensor.getData(),
      gpsSensor.getData());

  // Keep display on for viewing time
  delay(DISPLAY_TIME_MS);
}

void handleTimerWakeup()
{
  Serial.println("Woken up by timer!");

  // Initialize hardware
  initializeSensors();

  // Collect and log sensor data
  readAndPrintSensors();
}

/*
 * Main Arduino Functions
 */

void setup()
{
  time(&now);
  // Initialize serial communication for debugging
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }

  // Configure GPIO pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SD_CS_PIN, OUTPUT);

  // Turn on LED to indicate active state
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize SD card logging system
  if (!storage.initializeSDCard())
  {
    Serial.println("WARNING: SD card initialization failed - continuing without logging");
  }

  // Determine wake-up source and handle accordingly
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    // Wake-up triggered by button press
    handleButtonWakeup();
    break;

  case ESP_SLEEP_WAKEUP_TIMER:
    // Wake-up triggered by timer
    handleTimerWakeup();
    break;

  default:
    // First boot or unexpected wake-up
    Serial.println("First boot or unexpected wake-up");
    handleTimerWakeup();
    break;
  }

  // Enter sleep mode after handling wake-up
  goToSleep();
}

void loop()
{
  // This function will never be reached because the device
  // enters deep sleep at the end of setup()
}
