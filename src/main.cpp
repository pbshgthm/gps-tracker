#include <Arduino.h>
#include "display/Display.h"
#include "storage/Storage.h"
#include "gps/GPSSensor.h"
#include "environmental/EnvironmentalSensor.h"
#include "accelerometer/AccelerometerSensor.h"

const uint64_t uS_TO_S_FACTOR = 1000000;
const uint64_t TIME_TO_SLEEP = 10; // Sleep duration in seconds
#define BUTTON_PIN 15              // GPIO pin for button press

time_t timestamp;

GPSSensor gpsSensor;
EnvironmentalSensor envSensor;
AccelerometerSensor accSensor;
Display display;
Storage storage;

void sleep()
{
  Serial.println("Going to sleep");
  Serial.flush(); // Ensure all serial data is transmitted

  // Configure wake-up sources
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Timer-based wake-up
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 0);                  // Button-based wake-up (LOW signal)

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

void readSensors()
{
  gpsSensor.init();
  envSensor.init();
  accSensor.init();

  gpsSensor.powerOn();
  delay(2000);
  gpsSensor.powerOff();

  gpsSensor.read();
  envSensor.read();
  accSensor.read();

  gpsSensor.print();
  envSensor.print();
  accSensor.print();
}

void displayButtonPress()
{
  display.init();
  display.turnOn();
  readSensors();
  display.updateDisplay(envSensor.getData(), accSensor.getData(), gpsSensor.getData());
  delay(3000);
  display.turnOff();
  sleep();
}

void routineWakeUp()
{
  readSensors();
  storage.write(timestamp, envSensor.getData(), accSensor.getData(), gpsSensor.getData());
  sleep();
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  storage.init();
  time(&timestamp);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    displayButtonPress();
    break;

  case ESP_SLEEP_WAKEUP_TIMER:
    routineWakeUp();
    break;

  default:
    Serial.println("First boot or unexpected wake-up");
    routineWakeUp();
    break;
  }
}

void loop()
{
}
