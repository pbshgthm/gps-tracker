#include <Arduino.h>
#include "sensors/GPSSensor.h"
#include "sensors/EnvironmentalSensor.h"

// Define the wake-up interval (5 seconds)
const uint64_t uS_TO_S_FACTOR = 1000000; // Conversion factor for microseconds to seconds
const uint64_t TIME_TO_SLEEP = 10;       // Time ESP32 will go to sleep (in seconds)

GPSSensor gpsSensor;
EnvironmentalSensor envSensor;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Configure the wake-up source as a timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Ensure everything is powered down during sleep
  // Light sleep mode is selected by default with this configuration
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

  gpsSensor.init();
  envSensor.init();
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

  Serial.println("\nEntering sleep\n");
  gpsSensor.powerOff();
  esp_light_sleep_start();
}