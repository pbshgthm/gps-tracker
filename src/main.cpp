#include <Arduino.h>
#include "display/Display.h"
#include "storage/Storage.h"
#include "gps/GPSSensor.h"
#include "environmental/EnvironmentalSensor.h"
#include "accelerometer/AccelerometerSensor.h"

// --------------------- Constants & Config -----------------------
#define BUTTON_PIN_1 32
#define BUTTON_PIN_2 33

#define LONG_PRESS_THRESHOLD 2000 // milliseconds

const uint64_t uS_TO_S_FACTOR = 1000000ULL;
const uint64_t TIME_TO_SLEEP = 10; // in seconds

// --------------------- Global Objects ---------------------------
GPSSensor gpsSensor;
EnvironmentalSensor envSensor;
AccelerometerSensor accSensor;
Display display;
Storage storage;
time_t timestamp; // For storing time in logs

// --------------------- Function Prototypes ----------------------
void initAndReadSensors();
void logData();
void handleTimerWakeUp();
void goToSleep();
void handleWakeup();
void button1ShortPress();
void button1LongPress();
void button2ShortPress();
void button2LongPress();

// --------------------- Sensor & Logging -------------------------
void initAndReadSensors()
{
  // Initialize each sensor (custom code in your libraries)
  gpsSensor.init();
  envSensor.init();
  accSensor.init();

  // Example sequence: power on GPS, wait, power off, then read.
  gpsSensor.powerOn();
  delay(2000);
  gpsSensor.powerOff();

  // Read from each sensor
  gpsSensor.read();
  envSensor.read();
  accSensor.read();

  // Print for debugging
  gpsSensor.print();
  envSensor.print();
  accSensor.print();
}

void logData()
{
  storage.init();
  storage.write(timestamp, envSensor.getData(), accSensor.getData(), gpsSensor.getData());
  Serial.println("[INFO] Data logged to storage.");
}

// --------------------- Timer Wake-Up Logic ----------------------
void handleTimerWakeUp()
{
  Serial.println("[INFO] Timer wake-up or first boot/unexpected wake");
  initAndReadSensors();
  logData();
  goToSleep();
}

// --------------------- Button Press Routines --------------------
void button1ShortPress()
{
  // Equivalent to old handleSinglePress()
  Serial.println("[EVENT] Button 1 short press (Single Press)");

  // Use the display
  display.init();
  display.turnOn();
  display.greet();

  // Perform sensor reading & logging
  initAndReadSensors();
  logData();

  // Show data on display
  display.showData(envSensor.getData(), accSensor.getData(), gpsSensor.getData());
  delay(3000); // Keep data displayed for a bit

  display.turnOff();
  goToSleep();
}

void button1LongPress()
{
  // Equivalent to old handleLongPress()
  Serial.println("[EVENT] Button 1 long press");

  initAndReadSensors();
  logData();

  Serial.println("Long press action for Button 1.");
  delay(15000); // Example delay

  goToSleep();
}

void button2ShortPress()
{
  Serial.println("[EVENT] Button 2 short press");

  // You can replicate the same logic as Button 1 short or do something unique
  initAndReadSensors();
  logData();

  // For illustration, let's show data on the serial console only
  Serial.println("Short press action for Button 2.");
  delay(5000);

  goToSleep();
}

void button2LongPress()
{
  Serial.println("[EVENT] Button 2 long press");

  initAndReadSensors();
  logData();

  Serial.println("Long press action for Button 2.");
  delay(5000);

  goToSleep();
}

// --------------------- Sleep Helper -----------------------------
void goToSleep()
{
  Serial.println("[INFO] Going to deep sleep...");
  Serial.flush();

  // Enable wake-up from ext1 for pins 32 or 33 (HIGH level).
  esp_sleep_enable_ext1_wakeup((1ULL << BUTTON_PIN_1) | (1ULL << BUTTON_PIN_2),
                               ESP_EXT1_WAKEUP_ANY_HIGH);

  // Enable wake-up from timer, same as original
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Enter deep sleep.
  esp_deep_sleep_start();
}

// --------------------- Wake-Up Handler --------------------------
void handleWakeup()
{
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT1:
  {
    // Figure out which pin(s) triggered the wake-up
    uint64_t wakeup_pins = esp_sleep_get_ext1_wakeup_status();

    // BUTTON_PIN_1 check
    if (wakeup_pins & (1ULL << BUTTON_PIN_1))
    {
      // Measure press duration
      unsigned long pressStart = millis();
      while (digitalRead(BUTTON_PIN_1) == HIGH)
      {
        if (millis() - pressStart >= LONG_PRESS_THRESHOLD)
        {
          button1LongPress();
          return; // Stop after handling
        }
        delay(5);
      }
      // If we exit the loop, it was a short press
      button1ShortPress();
      return;
    }

    // BUTTON_PIN_2 check
    if (wakeup_pins & (1ULL << BUTTON_PIN_2))
    {
      unsigned long pressStart = millis();
      while (digitalRead(BUTTON_PIN_2) == HIGH)
      {
        if (millis() - pressStart >= LONG_PRESS_THRESHOLD)
        {
          button2LongPress();
          return;
        }
        delay(5);
      }
      button2ShortPress();
      return;
    }

    break;
  }

  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("[INFO] Woke up by timer");
    handleTimerWakeUp();
    break;

  default:
    Serial.println("[INFO] First boot or unexpected wake-up");
    // Just reuse your timer logic for first boot/unexpected
    handleTimerWakeUp();
    break;
  }
}

// --------------------- Setup & Loop -----------------------------
void setup()
{
  Serial.begin(115200);
  delay(100);

  // Configure the buttons as inputs with internal pull-down
  pinMode(BUTTON_PIN_1, INPUT_PULLDOWN);
  pinMode(BUTTON_PIN_2, INPUT_PULLDOWN);

  // Grab current time for logging
  time(&timestamp);

  // Determine wake-up cause and handle accordingly
  handleWakeup();

  // After handling wakeup, go to sleep
  goToSleep();
}

void loop()
{
  // Normally empty for deep-sleep scenarios
}