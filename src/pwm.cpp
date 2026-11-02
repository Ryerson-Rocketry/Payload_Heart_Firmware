#include "pwm.hpp"
#include "driver/mcpwm_prelude.h"

mcpwm_timer_handle_t timer = NULL;
mcpwm_oper_handle_t operatorHandle = NULL;
mcpwm_cmpr_handle_t comparator = NULL;
mcpwm_gen_handle_t generator = NULL;


esp_err_t mcpwmTimerInit(){
    mcpwm_timer_config_t timerConfig = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1 MHz resolution
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000,     // 1 kHz PWM frequency
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timerConfig, &timer));
    return ESP_OK;
}

esp_err_t mcpwmOperatorInit(){
    mcpwm_operator_config_t operatorConfig = {
        .group_id = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operatorConfig, &operatorHandle));

    // Connect the operator to the timer
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operatorHandle, timer));
    return ESP_OK;
}

esp_err_t mcpwmComparatorInit(){
    mcpwm_comparator_config_t comparatorConfig = {
        .flags = {
            .update_cmp_on_tez = true,
        },
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(operatorHandle, &comparatorConfig, &comparator));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 500)); // Duty cycle = 500/1000 * 100% = 50%
    return ESP_OK;
}

esp_err_t mcpwmGeneratorInit(){
    mcpwm_generator_config_t generatorConfig = {
        .gen_gpio_num = motorPin
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(operatorHandle, &generatorConfig, &generator));

    // comparator here instead?
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator,
        MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            MCPWM_TIMER_EVENT_EMPTY,
            MCPWM_GEN_ACTION_HIGH
        )));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator,
        MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            comparator,
            MCPWM_GEN_ACTION_LOW
         )));
    return ESP_OK;
}

esp_err_t setDutyCycle(uint8_t dutyCyclePercent){
    if (dutyCyclePercent > 100) {
        dutyCyclePercent = 100;
    }
    return mcpwm_comparator_set_compare_value(comparator, dutyCyclePercent * 10);
}
