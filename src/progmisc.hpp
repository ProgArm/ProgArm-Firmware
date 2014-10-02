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
#include "stm32f10x_gpio.h"

class Pin {
public:
    GPIO_InitTypeDef pin;
    GPIO_TypeDef* gpio;
    bool activeHigh;

    Pin(GPIO_TypeDef* gpio, uint16_t GPIO_Pin, //
            GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode, bool activeHigh);
    //Pin(GPIO_TypeDef* gpio, GPIO_InitTypeDef pin);
    void init();
    void turnOn();
    void turnOff();
    int toggle();
    u16 read();
};
