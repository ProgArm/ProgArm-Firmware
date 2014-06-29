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
int presenceCounter = 0;

void InitializePresenceIndicator() // TODO use real PWM!
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 4000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 20;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void EnableTimerInterrupt() {
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void configureLed() {
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    for (int i = 0; i < 3; i++) {
        gpio.GPIO_Pin = LED_PINS[i];
        gpio.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_Init(LED_GPIO[i], &gpio);

        GPIO_WriteBit(LED_GPIO[i], LED_PINS[i], Bit_SET); // turn off (active low)
    }

    //
    InitializePresenceIndicator();
    EnableTimerInterrupt();
}

extern "C" void TIM2_IRQHandler() {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        if (flashlightStatus) {
            for (int i = 0; i < 3; i++)
                LED_GPIO[i]->BRR = LED_PINS[i];
        } else if (presenceStatus) {
            LED_GPIO[LED_GREEN]->BRR = LED_PINS[LED_GREEN];
            LED_GPIO[LED_GREEN]->BSRR = LED_PINS[LED_GREEN];
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        //GPIO_ReadOutputDataBit()
    }
}

void notifyAboutError(DEVICE_ERROR error) {
    GPIO_WriteBit(LED_GPIO[LED_RED], LED_PINS[LED_RED], Bit_RESET);
}

void presenceToggle() {
    presenceStatus = !presenceStatus;
}

void flashlightToggle() {
    flashlightStatus = !flashlightStatus;
    flashlightSwitchPower(flashlightStatus);
}

void flashlightSwitchPower(bool turnOn) { // TODO change flashlightStatus
    for (int i = 0; i < 3; i++)
        GPIO_WriteBit(LED_GPIO[i], LED_PINS[i], turnOn ? Bit_RESET : Bit_SET);
}
