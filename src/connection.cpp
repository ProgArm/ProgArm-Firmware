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
#include <stm32f10x_usart.h>
#include <stm32f10x_spi.h>

#include "actions.hpp"
#include "connection.hpp"
#include "vibration.hpp"

const int OUTPUT_BUFFER_SIZE = 500;
uint8_t outputBuffer[OUTPUT_BUFFER_SIZE] = { }; // TODO separate buffer into class?
int outputBufferHead = 0;
int outputBufferTail = 0;

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
    PIN_BLUETOOTH_KEY.init();
    PIN_BLUETOOTH_POWER.init();
    PIN_BLUETOOTH_POWER.turnOn();

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

    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    NVIC_Configuration();
}

void processOutgoingData() {
    if (outputBufferHead != outputBufferTail)
        if (USART_GetFlagStatus(USART1, USART_FLAG_TXE)) {
            USART_SendData(USART1, outputBuffer[outputBufferHead++]);
            if (outputBufferHead >= OUTPUT_BUFFER_SIZE)
                outputBufferHead = 0;
        }
    if (outputBufferHead != outputBufferTail)
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

int reserveBufferPart(int neededSize) {
    __disable_irq();
    int returnPosition = outputBufferTail;
    outputBufferTail += neededSize;
    __enable_irq();
    return returnPosition;
}

void printPlain(const char* str) {
    int size = 0;
    for (; str[size]; size++)
        ;

    int start = reserveBufferPart(size + 2); // 'T' + str + \0
    outputBuffer[start] = 'T';
    int location = start;
    for (int i = 0; i < size + 1; i++) {
        if (++location >= OUTPUT_BUFFER_SIZE)
            location = 0;
        outputBuffer[location] = str[i];
    }
}

void printSensor(uint8_t sensor, const char* str) {
    int size = 0; // TODO !!! copy-pasted code from printPlain
    for (; str[size]; size++)
        ;

    int start = reserveBufferPart(size + 3); // 'C' + '?' + str + \0
    outputBuffer[start] = 'T';
    outputBuffer[start + 1] = sensor;
    int location = start + 1;
    for (int i = 0; i < size + 1; i++) {
        if (++location >= OUTPUT_BUFFER_SIZE)
            location = 0;
        outputBuffer[location] = str[i];
    }
}

void print(const char* str) {
    int size = 0; // TODO !!! copy-pasted code from printPlain
    for (; str[size]; size++)
        ;

    int start = reserveBufferPart(size);
    int location = start;
    for (int i = 0; i < size; i++) {
        outputBuffer[location++] = str[i];
        if (location >= OUTPUT_BUFFER_SIZE)
            location = 0;
    }

    processOutgoingData();
}

void printAction(uint8_t action) {
    int location = reserveBufferPart(2);
    outputBuffer[location] = 'L';
    outputBuffer[location + 1] = action;
    processOutgoingData();
}

uint8_t clientGet() {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        ;
    return (uint8_t) USART_ReceiveData(USART1);
}

extern "C" void USART1_IRQHandler(void) {
    if (USART_GetFlagStatus(USART1, USART_FLAG_TXE)) { // copy-pasted from processOutgoingData
        USART_SendData(USART1, outputBuffer[outputBufferHead++]); // XXX what if that buffer was not filled yet?
        if (outputBufferHead >= OUTPUT_BUFFER_SIZE)
            outputBufferHead = 0;

        if (outputBufferHead == outputBufferTail)
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
    //outputBufferTail++;
    //USART_ClearITPendingBit(USART1,USART_IT_TXE);
    //processOutgoingData();
}

void processIncomingData() { // TODO use interrupts to process data
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        return; // no data to read

    uint8_t received = (uint8_t) USART_ReceiveData(USART1);
    switch (received) {
    case 'p':
        //clientPut('P'); // TODO
        break;
    case 'E':
        changeState(clientGet()); // XXX what if client disconnects exactly at this moment?
        break;
    case 'V':
        setVibration(0xFFFF / 255 * clientGet());
        break;
    default:
        char buffer[60]; // XXX use some existing buffer instead? // NO, use outgoing buffer when output becomes interrupt-driven
        sprintf(buffer, "Error: Skipping unexpected byte: %d\n", received);
        printPlain(buffer);
        break;
    }
}
