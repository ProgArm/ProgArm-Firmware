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

#include "ring.hpp"

#include <misc.h>
#include <stm32f10x_exti.h>
#include <algorithm>
#include <cstdint>

#include "../core/keys.hpp"
#include "../core/notificationManager.hpp"
#include "../core/Notification.hpp"
#include "../core/timing.hpp"

volatile bool pressed[RING_BUTTON_COUNT] = { };
volatile bool wasPressed[RING_BUTTON_COUNT] = { };
int lastPress[RING_BUTTON_COUNT] = { };
Notification* pressNotifications[RING_BUTTON_COUNT] = { };
Notification* buttonsResetNotification;

static const int DEBOUNCE_MILLISECONDS = 10;
static const int CLICK_SHORT_MILLISECONDS = 50;
static const int CLICK_LONG_MILLISECONDS = 280;
volatile u16 action = 0;
volatile int clicks = 0;

static const int CLICK_TIMEOUT_MILLISECONDS = 500;

void configureRing() {
    PIN_RING_BUTTON_1.init();
    PIN_RING_BUTTON_2.init();

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

    pressNotifications[0] = new Notification(0, 0, 0x00FF, 1000, 2);
    pressNotifications[1] = new Notification(0, 0x00FF, 0, 1000, 2);
    for (auto &curNotification : pressNotifications)
        addNotification(curNotification);
    buttonsResetNotification = new Notification(0x00FF, 0, 0, 1000, 3);
    addNotification(buttonsResetNotification);
}

void resetButtons() {
    if (milliseconds - std::max(lastPress[0], lastPress[1]) < CLICK_TIMEOUT_MILLISECONDS)
        return; // no reset needed

    if (clicks != 0) {
        buttonsResetNotification->turnOn();
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
        processKey(action);
        action = 0;
        clicks = 0;
    } else
        action *= 6; // shift left in base 6
}

void processInterrupt(int buttonId) {
    int delta = milliseconds - lastPress[buttonId];
    if (delta > DEBOUNCE_MILLISECONDS) {
        pressed[buttonId] = !pressed[buttonId];
        if (pressed[buttonId]) { // press
            wasPressed[buttonId] = true;
            pressNotifications[buttonId]->turnOn();
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
