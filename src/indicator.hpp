#pragma once

#include <stm32f10x_gpio.h>

enum LED_COLOR {
    LED_RED=0, LED_GREEN=1, LED_BLUE=2
};

enum DEVICE_ERROR {
    NO_HSE
};

static GPIO_TypeDef * LED_GPIO[] = { GPIOB, GPIOB, GPIOB };
static uint16_t LED_PINS[] = { GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5 };

void configureLed();
void notifyAboutError(DEVICE_ERROR error);

void presenceToggle();
void flashlightToggle();
void flashlightSwitchPower(bool turnOn);

