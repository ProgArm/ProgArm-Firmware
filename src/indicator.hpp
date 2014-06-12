/* Copyright (C) 2014  Alex-Daniel Jakimenko <alex.jakimenko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */
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

