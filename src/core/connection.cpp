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

#include "connection.hpp"

#include <misc.h>
#include <stdarg.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <cstdio>
#include <queue>

#include "../systems/vibration.hpp"
#include "actions.hpp"

namespace {
std::queue<u8> outputBuffer;
std::queue<u8> inputBuffer;
}

void NVIC_Configuration(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Place the vector table into FLASH */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); // XXX why?
    /* Enabling interrupt from USART1 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void configureConnection() {
    //PIN_BLUETOOTH_KEY.init();
    //PIN_BLUETOOTH_POWER.init();
    //PIN_BLUETOOTH_POWER.turnOn();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef gpio;
    // Rx
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    // Tx
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = BLUETOOTH_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_Configuration();
}

void processOutgoingData() { // call this every time you push to outputBuffer
    if (!outputBuffer.empty())
        if (USART_GetFlagStatus(USART1, USART_FLAG_TXE)) {
            USART_SendData(USART1, outputBuffer.front());
            outputBuffer.pop();
        }
    if (!outputBuffer.empty())
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void printData(char startChar, const char* str) {
    __disable_irq();
    outputBuffer.push(startChar);
    for (int curPos = 0; str[curPos]; curPos++)
        outputBuffer.push(str[curPos]);
    outputBuffer.push('\0');
    __enable_irq();
    processOutgoingData();
}

void printPlain(const char* str) {
    printData('T', str);
}

void print(const char* str) {
    __disable_irq();
    for (int curPos = 0; str[curPos]; curPos++)
        outputBuffer.push(str[curPos]);
    __enable_irq();
    processOutgoingData();
}

void print(int n_args, ...) {
    va_list ap;
    va_start(ap, n_args);
    __disable_irq();
    for (int i = 1; i <= n_args; i++)
        outputBuffer.push(va_arg(ap, int));
    __enable_irq();
    va_end(ap);
    processOutgoingData();
}

void printAction(u8 action) {
    __disable_irq();
    outputBuffer.push('L');
    outputBuffer.push(action);
    __enable_irq();
    processOutgoingData();
}

extern "C" void USART1_IRQHandler(void) {
    if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) && !outputBuffer.empty()) {
        USART_SendData(USART1, outputBuffer.front());
        outputBuffer.pop();
        if (outputBuffer.empty())
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
        inputBuffer.push(USART_ReceiveData(USART1));
        processIncomingData();
    }
}

void sendBatteryData() {
    // TODO
    //print(7, 'B', charge >> 8, charge & 0xFF, voltage >> 8, voltage & 0xFF, temperature >> 8, temperature & 0xFF);
}

void processIncomingData() { // TODO make it shorter
    switch (inputBuffer.front()) {
    case 'p':
        inputBuffer.pop();
        print("P");
        break;
    case 'b':
        inputBuffer.pop();
        sendBatteryData();
        break;
    case 'E':
        if (inputBuffer.size() < 2)
            break;
        inputBuffer.pop();
        changeState(inputBuffer.front()); // XXX what if client disconnects exactly at this moment?
        inputBuffer.pop();
        break;
    case 'V':
        if (inputBuffer.size() < 2)
            break;
        inputBuffer.pop();
        setVibration(0xFFFF / 255 * inputBuffer.front());
        inputBuffer.pop();
        break;
    case 'l':
        if (inputBuffer.size() < 2)
            break;
        inputBuffer.pop();
        processAction(inputBuffer.front());
        inputBuffer.pop();
        break;
    default:
        char buffer[60]; // XXX use some existing buffer instead? // NO, use outgoing buffer when output becomes interrupt-driven
        sprintf(buffer, "Error: Skipping unexpected byte: %d\n", inputBuffer.front());
        inputBuffer.pop();
        printPlain(buffer);
        break;
    }
}
