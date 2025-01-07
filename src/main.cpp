#include <Arduino.h>
#include "display/Display.h"
#include "storage/Storage.h"
#include "gps/GPSSensor.h"
#include "environmental/EnvironmentalSensor.h"
#include "accelerometer/AccelerometerSensor.h"

// --------------------- Constants & Config -----------------------
#define BUZZER_PIN 12
#define BUTTON_PIN_1 32
#define BUTTON_PIN_2 33
#define DOUBLE_PRESS_WINDOW 500   // milliseconds
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

void beepOnce()
{
  ledcWriteTone(0, 2000); // Play 2 kHz tone
  delay(100);             // for 100 ms
  ledcWriteTone(0, 0);    // Stop buzzer
}

void beepTwice()
{
  beepOnce();
  delay(100);
  beepOnce();
}

// --------------------- Sleep Helper -----------------------------
void goToSleep()
{
  Serial.println("[INFO] Going to deep sleep...");
  Serial.flush();

  // Enable wake-up from button (EXT0).
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0);

  // Enable wake-up from timer.
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Enter deep sleep.
  esp_deep_sleep_start();
}

// --------------------- Sensor & Logging --------------------------
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

// --------------------- Press Handlers ---------------------------

void handleSinglePress()
{
  beepOnce();
  Serial.println("[EVENT] Single Press");
  display.init();
  display.turnOn();
  display.greet();

  initAndReadSensors();
  logData();

  // Update the display
  display.showData(envSensor.getData(), accSensor.getData(), gpsSensor.getData());
  delay(5000); // Display the data for a few seconds

  display.turnOff();
  goToSleep();
}

void handleDoublePress()
{
  beepTwice();
  Serial.println("[EVENT] Double Press");
  initAndReadSensors();
  logData();

  Serial.println("double press to console");
  goToSleep();
}

void handleLongPress()
{
  Serial.println("[EVENT] Long Press");
  initAndReadSensors();
  logData();

  Serial.println("long press to the monitor");
  delay(15000);

  goToSleep();
}

// --------------------- Timer Wake-Up Logic -----------------------
void handleTimerWakeUp()
{
  Serial.println("[INFO] Timer wake-up or first boot/unexpected wake");
  initAndReadSensors();
  logData();
  goToSleep();
}

// --------------------- Button Press Detection --------------------

void detectButtonPress()
{
  unsigned long pressStart = millis();

  // 1) Check Long Press (button is still LOW upon wake-up).
  while (digitalRead(BUTTON_PIN_1) == LOW)
  {
    if (millis() - pressStart >= LONG_PRESS_THRESHOLD)
    {
      // It's a Long Press
      handleLongPress();
      return; // Stop after handling
    }
    delay(5);
  }

  // 2) If we exit the loop, button was released before LONG_PRESS_THRESHOLD.
  //    Let's see if there's a 2nd press within DOUBLE_PRESS_WINDOW.
  bool secondPressDetected = false;
  unsigned long firstReleaseTime = millis();

  while (millis() - firstReleaseTime < DOUBLE_PRESS_WINDOW)
  {
    if (digitalRead(BUTTON_PIN_1) == LOW)
    {
      // Wait for user to release second press
      while (digitalRead(BUTTON_PIN_1) == LOW)
      {
        delay(5);
      }
      secondPressDetected = true;
      break;
    }
    delay(5);
  }

  // 3) Decide single vs double press
  if (secondPressDetected)
  {
    handleDoublePress();
  }
  else
  {
    handleSinglePress();
  }
}

// --------------------- Setup & Loop -----------------------------
void setup()
{
  Serial.begin(115200);
  delay(100);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);

  ledcSetup(0, 2000, 8);        // Channel 0, 2 kHz frequency, 8-bit resolution
  ledcAttachPin(BUZZER_PIN, 0); // Attach channel 0 to BUZZER_PIN

  // Get current time (for logging)
  time(&timestamp);

  // Determine wake-up cause
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("[INFO] Woke up by button press (EXT0)");
    detectButtonPress();
    break;

  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("[INFO] Woke up by timer");
    handleTimerWakeUp();
    break;

  default:
    // First boot or other cause
    Serial.println("[INFO] First boot or unexpected wake-up");
    handleTimerWakeUp();
    break;
  }
}

void loop()
{
  // Normally empty for deep-sleep scenarios
}
