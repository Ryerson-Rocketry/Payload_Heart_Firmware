#ifndef PWM_HPP
#define PWM_HPP
#include "driver/mcpwm_prelude.h"

constexpr int motorPin = 25;

extern mcpwm_timer_handle_t timer;

esp_err_t mcpwmTimerInit();
esp_err_t mcpwmOperatorInit();  
esp_err_t mcpwmComparatorInit();
esp_err_t mcpwmGeneratorInit();

esp_err_t setDutyCycle(uint8_t dutyCyclePercent);

#endif // PWM_H