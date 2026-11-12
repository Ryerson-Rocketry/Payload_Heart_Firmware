#include <Arduino.h>
#include <Wire.h>
#include <stdio.h>
#include "driver/mcpwm_prelude.h"
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "pwm.hpp"
#include "esp_task_wdt.h"
#include "esp_err.h"

#define TWDT_TIMEOUT 5000 // timeout within ms 
#define TASK_RESET_PERIOD 2000


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

  { 
  #if !CONFIG_ESP_TASK_WDT_INIT //if twdt not initialized automatically, manually initalizing it
    esp_task_wdt_config_t wdt_config = {
      .timeout_ms = TWDT_TIMEOUT,
      .idle_core_mask = ESP_TASK_WDT_IDLE_CORE_DEFAULT,
      .trigger_panic = true,
    };
    ESP_ERROR_CHECK(esp_task_wdt_init(&wdt_config));
    printf("TWDT initialized with timeout of %d ms\n", TWDT_TIMEOUT);
  #endif
  }; 

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

  ESP_ERROR_CHECK(esp_task_wdt_add(NULL)); // adding the current task to the wdt 
  printf("subscried to TWDT\n"); 

  for(;;){
    // use pwm to control the pump
    Serial.println("Motor ON");
    ESP_ERROR_CHECK(setDutyCycle(highDutyCycle));
    vTaskDelay(pdMS_TO_TICKS(1000));

    // --- MOTOR OFF (e.g., Rest for 3 seconds) ---
    Serial.println("Motor OFF (0V)");
    ESP_ERROR_CHECK(setDutyCycle(lowDutyCycle));
    vTaskDelay(pdMS_TO_TICKS(1666));

    ESP_ERROR_CHECK(esp_task_wdt_reset()); // reset task 
    }
}

// find a way to use iwdt for esp32