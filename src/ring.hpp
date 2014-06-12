#pragma once
#include <stdint.h>
#include <stm32f10x_gpio.h>

static const int RING_BUTTON_COUNT = 2;
static const uint16_t RING_BUTTON_PINS[] = { GPIO_Pin_0, GPIO_Pin_12 };

void configureRing();
void resetButtons();
//void buttonRelease();
