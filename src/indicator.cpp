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
#include <stm32f10x_gpio.h>
#include "indicator.hpp"

bool flashlightStatus = false;
bool presenceStatus = false;

void InitializePresenceIndicator() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM2->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E); // select used pins
    TIM2->CCMR1 |= (TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); // inverse PWM
    TIM2->CCMR2 |= (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2);
    TIM2->CCMR2 |= (TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);
    TIM2->CR1 |= TIM_CR1_CEN; // enable counter

    TIM2->CCR2 = 0; // turn it off
    TIM2->CCR3 = 0;
    TIM2->CCR4 = 0;
}

void configureLed() {
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &gpio);

    InitializePresenceIndicator();
}

void notifyAboutError(DEVICE_ERROR error) {
//GPIO_WriteBit(LED_GPIO[LED_RED], LED_PINS[LED_RED], Bit_RESET); // TODO
}

void presenceToggle() {
    presenceStatus = !presenceStatus;
}

void flashlightToggle() {
    flashlightStatus = !flashlightStatus;
    flashlightSwitchPower(flashlightStatus);
}

void flashlightSwitchPower(bool turnOn) { // TODO change flashlightStatus
    TIM2->CCR2 = turnOn ? 0xFFFF : 0;
    TIM2->CCR3 = turnOn ? 0xFFFF : 0;
    TIM2->CCR4 = turnOn ? 0xFFFF : 0;
}
