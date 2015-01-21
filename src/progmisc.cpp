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
#include "progmisc.hpp"

Pin::Pin(GPIO_TypeDef* gpio, uint16_t GPIO_Pin, //
        GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode, bool activeHigh) {
    this->gpio = gpio;
    this->pin.GPIO_Pin = GPIO_Pin;
    this->pin.GPIO_Speed = GPIO_Speed;
    this->pin.GPIO_Mode = GPIO_Mode;
    this->activeHigh = activeHigh;
}

void Pin::init() {
    GPIO_Init(gpio, &pin);
    if (pin.GPIO_Mode == GPIO_Mode_Out_PP || pin.GPIO_Mode == GPIO_Mode_Out_OD)
        turnOff();
}

void Pin::turnOn() {
    // TODO write bit directly without GPIO_WriteBit function?
    GPIO_WriteBit(gpio, pin.GPIO_Pin, activeHigh ? Bit_SET : Bit_RESET);
}

void Pin::turnOff() {
    // TODO write bit directly without GPIO_WriteBit function?
    GPIO_WriteBit(gpio, pin.GPIO_Pin, activeHigh ? Bit_RESET : Bit_SET);
}

int Pin::toggle() {
    int currentState = GPIO_ReadOutputDataBit(gpio, pin.GPIO_Pin);
    GPIO_WriteBit(gpio, pin.GPIO_Pin, currentState == Bit_SET ? Bit_RESET : Bit_SET);
    return currentState;
}

u16 Pin::read() {
    // TODO assert that the pin is in input mode?
    if (activeHigh)
        return GPIO_ReadInputDataBit(gpio, pin.GPIO_Pin);
    else
        return GPIO_ReadInputDataBit(gpio, pin.GPIO_Pin) == Bit_SET ? Bit_RESET : Bit_SET;
}
