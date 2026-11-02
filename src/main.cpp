#include <Arduino.h>
#include <Wire.h>
#include <stdio.h>
#include "driver/mcpwm_prelude.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "pwm.hpp"

TaskHandle_t vReadTempHandle;
TaskHandle_t vReadPressureHandle;
TaskHandle_t vReadFlowRateHandle;  
TaskHandle_t vPumpHeartHandle;

SemaphoreHandle_t gI2CMutex;

// Task headers
// void vReadTemp(void *argument);
// void vReadPressure(void *argument);
// void vReadFlowRate(void *argument);
void vPumpHeart(void *argument);

void setup() {
  Serial.begin(115200);

  gI2CMutex = xSemaphoreCreateMutex();

  // check if semaphores were created successfully  
  configASSERT(gI2CMutex != NULL);

  ESP_ERROR_CHECK(mcpwmTimerInit());
  ESP_ERROR_CHECK(mcpwmOperatorInit());
  ESP_ERROR_CHECK(mcpwmComparatorInit());
  ESP_ERROR_CHECK(mcpwmGeneratorInit());
  ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
  ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

  // Create tasks (Function, name of task, stack size, parameters, priority, handle)
  // xTaskCreate(vReadTemp, "ReadTemperature", 1024, NULL, 2, &vReadTempHandle);
  // xTaskCreate(vReadPressure, "ReadPressure", 1024, NULL, 2, &vReadPressureHandle);
  // xTaskCreate(vReadFlowRate, "ReadFlowRate", 1024, NULL, 2, &vReadFlowRateHandle);
  xTaskCreate(vPumpHeart, "PumpHeart", 1024, NULL, 1, &vPumpHeartHandle);

  Wire.begin(); 
  Serial.println("Starting ESP32!"); 
}

void loop() {
  // leave empty
}

// void vReadTemp(void *argument){
//   // setup
//   for(;;){
//     if (xSemaphoreTake(gI2CMutex, 100) == pdTRUE)
//     {
//       // read the sensor using i2c
//       xSemaphoreGive(gI2CMutex);
//     }
//   }
// }

// void vReadPressure(void *argument){
//   // setup
//   for(;;){
//     if (xSemaphoreTake(gI2CMutex, 100) == pdTRUE)
//     {
//       // read the sensor using i2c
//       xSemaphoreGive(gI2CMutex);
//     }
//   }
// }

// void vReadFlowRate(void *argument){
//   // setup
//   for(;;){
//     // read the sensor using spi
//   }
// } 

void vPumpHeart(void *argument){
  // setup, needs to be optimized as a percentage
  uint8_t highDutyCycle = 50; // 50% duty cycle
  uint8_t lowDutyCycle = 0;   // 0% duty cycle

  for(;;){
    // use pwm to control the pump
    Serial.println("Motor ON");
    ESP_ERROR_CHECK(setDutyCycle(highDutyCycle));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // --- MOTOR OFF (e.g., Rest for 3 seconds) ---
    Serial.println("Motor OFF (0V)");
    ESP_ERROR_CHECK(setDutyCycle(lowDutyCycle));
    vTaskDelay(pdMS_TO_TICKS(1666));
  }
}

// find a way to use iwdt for esp32