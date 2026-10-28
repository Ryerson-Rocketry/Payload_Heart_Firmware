#include <Arduino.h>
#include <Wire.h>
#include <stdio.h>
#include "freertos/freertos.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

TaskHandle_t vReadTempHandle;
TaskHandle_t vReadPressureHandle;
TaskHandle_t vReadFlowRateHandle;  
TaskHandle_t vPumpHeartHandle;

SemaphoreHandle_t gI2CMutex;

// Task headers
void vReadTemp(void *argument);
void vReadPressure(void *argument);
void vReadFlowRate(void *argument);
void vPumpHeart(void *argument);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  gI2CMutex = xSemaphoreCreateMutex();

  // check if semaphores were created successfully  
  ConfigASSERT(gI2CMutex != NULL);

  // Create tasks (Function, name of task, stack size, parameters, priority, handle)
  xTaskCreate(vReadTemp, "ReadTemperature", 1024, NULL, 2, &vReadTempHandle);
  xTaskCreate(vReadPressure, "ReadPressure", 1024, NULL, 2, &vReadPressureHandle);
  xTaskCreate(vReadFlowRate, "ReadFlowRate", 1024, NULL, 2, &vReadFlowRateHandle);
  xTaskCreate(vPumpHeart, "PumpHeart", 1024, NULL, 1, &vPumpHeartHandle);

  Wire.begin();
}

void loop() {
  // leave empty
}

void vReadTemp(void *argument){
  // setup
  for(;;){
    if (xSemaphoreTake(gI2CMutex, 100) == pdTRUE)
    {
      // read the sensor using i2c
      xSemaphoreGive(gI2CMutex);
    }
  }
}

void vReadPressure(void *argument){
  // setup
  for(;;){
    if (xSemaphoreTake(gI2CMutex, 100) == pdTRUE)
    {
      // read the sensor using i2c
      xSemaphoreGive(gI2CMutex);
    }
  }
}

void vReadFlowRate(void *argument){
  // setup
  for(;;){
    // read the sensor using spi
  }
} 

void vPumpHeart(void *argument){
  // setup
  for(;;){
    // use pwm to control the pump
  }
}

// find a way to use iwdt for esp32