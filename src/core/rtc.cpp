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

#include <misc.h>
#include <stm32f10x.h>
#include <stm32f10x_bkp.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_rtc.h>
#include <tuple>

namespace rtc {

void configuration(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE); // Allow access to BKP Domain
    BKP_DeInit(); // Reset Backup Domain // TODO move to backup.cpp?

    RCC_LSEConfig(RCC_LSE_ON); // Enable LSE
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        ; // Wait till LSE is ready

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();

    // Set RTC prescaler: set RTC period to 1sec
    RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
    RTC_WaitForLastTask();
}

void setup() {
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) { //RTC not configured yet
        configuration();
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    } else {
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
            //Power On Reset occurred...
        } else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
            //External Reset occurred...
        }
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_ALR, ENABLE);
        RTC_WaitForLastTask(); // Wait until last write operation on RTC registers has finished
        RCC_ClearFlag();
    }

    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    // Enable the RTC Interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //RTC_ClearITPendingBit(RTC_IT_ALR);
    RCC_ClearFlag();
}

std::tuple<int, int, int> getTime() {
    int value = RTC_GetCounter();
    int hours = value / 3600;
    int minutes = (value % 3600) / 60;
    int seconds = (value % 3600) % 60;
    return std::make_tuple(hours, minutes, seconds);
}

}

extern "C" void RTC_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
        RTC_WaitForLastTask();
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_WaitForLastTask();
        // TODO
    }
}
