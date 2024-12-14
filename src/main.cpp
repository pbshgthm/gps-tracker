#include <Arduino.h>
#include "display/Display.h"
#include "gps/GPSSensor.h"
#include "environmental/EnvironmentalSensor.h"
#include "accelerometer/AccelerometerSensor.h"

// Define the wake-up interval (5 seconds)
const uint64_t uS_TO_S_FACTOR = 1000000; // Conversion factor for microseconds to seconds
const uint64_t TIME_TO_SLEEP = 10;       // Time ESP32 will go to sleep (in seconds)

GPSSensor gpsSensor;
EnvironmentalSensor envSensor;
AccelerometerSensor accSensor;

Display display;

void setup()
{
  Serial.begin(115200);
  delay(100);

  // Configure the wake-up source as a timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Ensure everything is powered down during sleep
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

  gpsSensor.init();
  envSensor.init();
  accSensor.init();
  display.init();
  display.turnOn();
}

void loop()
{
  Serial.println("\nWaking up from sleep\n");
  gpsSensor.powerOn();
  delay(2000);
  gpsSensor.read();
  gpsSensor.print();
  envSensor.read();
  envSensor.print();
  accSensor.read();
  accSensor.print();

  display.updateDisplay(envSensor.getData(), accSensor.getData(), gpsSensor.getData());

  Serial.println("\nEntering sleep\n");
  gpsSensor.powerOff();
  esp_light_sleep_start();
}