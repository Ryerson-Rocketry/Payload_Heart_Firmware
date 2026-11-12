#include <Arduino.h>
#include "pressure.hpp"

namespace {
    const uint8_t pressurePin = 34;
    const float pressureDividerGain = 2.0;
    const float pressureMinVolts = 0.5;
    const float pressureMaxVolts = 4.5;
    const float pressureMaxPsi = 100.0;
}

void pressureInit(){
  analogReadResolution(12);

  // First read initializes the pin as an ADC channel.
  analogRead(pressurePin);

  analogSetPinAttenuation(pressurePin, ADC_11db);
}

float readPressurePsi(){
  float adcVolts = analogReadMilliVolts(pressurePin) / 1000.0f; // Convert mV to V
  Serial.printf("ADC pin: %.3f V\n", adcVolts);

  float sensorVolts = adcVolts * pressureDividerGain; // Scale up to full voltage from divider circuit

  float pressurePsi = (sensorVolts - pressureMinVolts)  // Convert voltage to psi (assuming linear relationship)
                      / (pressureMaxVolts - pressureMinVolts)
                      * pressureMaxPsi;

  return pressurePsi;
}