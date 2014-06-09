#pragma once

#include <stdint.h>
#include <stm32f10x_gpio.h>

static const uint32_t CLOCK_FREQUENCY = 8000000;
static const uint16_t POWER_PERIPHERALS = GPIO_Pin_1;

void configureDevice();
void configurePeripheralPower();

void peripheralsSwitchPower(bool turnOn);
