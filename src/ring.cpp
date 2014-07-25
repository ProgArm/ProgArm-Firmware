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
#include <stm32f10x_exti.h>
#include <misc.h>
#include <algorithm>

#include "ring.hpp"
#include "indicator.hpp"
#include "timing.hpp"
#include "connection.hpp"
#include "actions.hpp"

volatile bool pressed[RING_BUTTON_COUNT] = { };
volatile bool wasPressed[RING_BUTTON_COUNT] = { };
int lastPress[RING_BUTTON_COUNT] = { };

static const int DEBOUNCE_MILLISECONDS = 10;
static const int CLICK_SHORT_MILLISECONDS = 50;
static const int CLICK_LONG_MILLISECONDS = 280;
volatile uint16_t action = 0;
volatile int clicks = 0;

static const int CLICK_TIMEOUT_MILLISECONDS = 500;

void configureRing() {
    //PIN_RING_BUTTON_1.init();
    //PIN_RING_BUTTON_2.init();

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &gpio);



    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Second button
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

void resetButtons() {
    if (milliseconds - std::max(lastPress[0], lastPress[1]) < CLICK_TIMEOUT_MILLISECONDS)
        return; // no reset needed

    if (clicks != 0) {
        //LED_GPIO[LED_GREEN]->BRR = LED_PINS[LED_GREEN]; //TODO
        //LED_GPIO[LED_GREEN]->BSRR = LED_PINS[LED_GREEN];
        for (int i = 0; i < RING_BUTTON_COUNT; i++) {
            pressed[i] = false;
            wasPressed[i] = false;
            //lastPress[i]=0;
        }
        action = 0;
        clicks = 0;
    }
}

void buttonRelease() { // XXX it is meant to work with two buttons only, add support for more buttons?
    if (!wasPressed[0] && !wasPressed[1]) // if this release was already processed.
        return;

    int delta1 = milliseconds - lastPress[0];
    int delta2 = milliseconds - lastPress[1];
    int minDelta = delta1 < delta2 ? delta1 : delta2;

    bool shortpress = minDelta < (CLICK_LONG_MILLISECONDS + CLICK_SHORT_MILLISECONDS) / 2;

    clicks++;

    if (shortpress) {
        if (wasPressed[0] && !wasPressed[1])
            action += 0;
        else if (!wasPressed[0] && wasPressed[1])
            action += 2;
        else if (wasPressed[0] && wasPressed[1])
            action += 4;
    } else { // long press
        if (wasPressed[0] && !wasPressed[1])
            action += 1;
        else if (!wasPressed[0] && wasPressed[1])
            action += 3;
        else if (wasPressed[0] && wasPressed[1])
            action += 5;
    }

    wasPressed[0] = false;
    wasPressed[1] = false;

    if (clicks == 2) {
        processAction(action);
        printAction(action);
        action = 0;
        clicks = 0;
    } else
        action *= 6; // shift left in base 6
}

void processInterrupt(int buttonId) {
    int delta = milliseconds - lastPress[buttonId];
    if (milliseconds == 0)
        ; //GPIO_WriteBit(LED_GPIO[0], LED_PINS[0], Bit_RESET); // TODO
    if (delta > DEBOUNCE_MILLISECONDS) {
        pressed[buttonId] = !pressed[buttonId];
        if (pressed[buttonId]) { // press
            wasPressed[buttonId] = true;
            //TODO
            if (buttonId == 0) {
                TIM2->CCR2 = 0xFFFF;
                TIM2->CCR2 = 0;
            }
            else if (buttonId == 1) {
                TIM2->CCR3 = 0xFFFF;
                TIM2->CCR3 = 0;
            }
        } else { // release
            buttonRelease();
        }
        lastPress[buttonId] = milliseconds;
    }
}

extern "C" void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
        processInterrupt(0);
    EXTI_ClearITPendingBit(EXTI_Line0);
}

extern "C" void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
        processInterrupt(1);
    EXTI_ClearITPendingBit(EXTI_Line1);
}
