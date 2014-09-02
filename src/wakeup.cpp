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
#include "wakeup.hpp"
#include "timing.hpp"
#include <queue>
#include <stm32f10x_tim.h>

std::priority_queue<int, std::vector<int>, std::greater<int> > wakeUpQueue;

void setWakeTimer() {
    while (wakeUpQueue.size() > 0 && wakeUpQueue.top() <= milliseconds)
        wakeUpQueue.pop(); // remove queued wakeups that are too late
    if (wakeUpQueue.size() == 0)
        return; // there is no wakeup queued
    int next = wakeUpQueue.top();
    TIM4->CR1 &= ~TIM_CR1_CEN;
    TIM4->CNT = 0;
    TIM4->ARR = next - milliseconds;
    TIM4->CR1 |= TIM_CR1_CEN;
}

void addWakeUp(int ms) {
    wakeUpQueue.push(milliseconds + ms);
}

void configureWakeups() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 8000; // 8000 to tick every millisecond
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 0xFFFF;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &timerInitStructure);
    //TIM4->CR1 |= TIM_CR1_CEN;
    // TIM4->CR1 |= // ARPE to disable auto-reload
    //TIM_Cmd(TIM4, ENABLE);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM4->CR1 &= ~TIM_CR1_CEN;

    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

extern "C" void TIM4_IRQHandler() {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM4->CR1 &= ~TIM_CR1_CEN;
        TIM4->CNT = 0;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}
