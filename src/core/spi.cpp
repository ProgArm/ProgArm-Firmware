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

#include "spi.hpp"

#include <misc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>
#include <sys/types.h>

namespace {

uint packetInputBytesLeft = 0;
uint packetOutputBytesLeft = 0;
uint packetByteId = 0;

std::queue<u8> outputBuffer;
std::queue<u8> inputBuffer;

// Send one byte over SPI (or 0 if output buffer is empty)
// This function should be called even if you want to READ a byte
// because SPI is working in full duplex
void spiPush() {
    if (packetByteId == 0 && !outputBuffer.empty() && outputBuffer.front() < outputBuffer.size()) { // without +1 because <
        packetOutputBytesLeft = outputBuffer.front();
        SPI_I2S_SendData(SPI1, outputBuffer.front()); // packet length
        outputBuffer.pop();
    } else if (packetOutputBytesLeft > 0) {
        SPI_I2S_SendData(SPI1, outputBuffer.front()); // data
        outputBuffer.pop();
        packetOutputBytesLeft--;
    } else
        // we have to send something because we are in Full-Duplex
        SPI_I2S_SendData(SPI1, 0);
}

void spiPull() {
    auto data = SPI_I2S_ReceiveData(SPI1);
    if (packetByteId == 0) {
        // byte 0 is a weird byte (according to datasheet it should be discarded)
    } else {
        if (packetByteId == 1) { // byte 1 is packet length
            packetInputBytesLeft = data;
            if (packetInputBytesLeft > 0) {
                inputBuffer.push(data);
            }
        } else if (packetInputBytesLeft > 0) {
            inputBuffer.push(data);
            packetInputBytesLeft--;
        }
    }
}

}

namespace spi {

void setup() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    // REQN and RDYN are specific to nRF8001
    PIN_REQN.init();
    PIN_RDYN.init();
    PIN_REQN.turnOff();

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; // XXX that's not the only place where we use EXTI9_5 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TODO refactoring & code style
    ///
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO check priorities
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    //SPI_CalculateCRC(SPI1, DISABLE);
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
    //SPI_SSOutputCmd(SPI1, ENABLE);
    SPI_Cmd(SPI1, ENABLE);
    //SPI_Cmd(SPI1, ENABLE);
}

}

extern "C" void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line7)) { // RDYN
        EXTI_ClearITPendingBit(EXTI_Line7);
        // This interrupt means that nRF8001 wants to tell us something
        // OR we want to tell something
        PIN_REQN.turnOn(); // Let's ask it to start a packet transmission!
        spiPush();
        //SPI_Cmd(SPI1, ENABLE);
    }
}

extern "C" void SPI1_IRQHandler(void) {
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)) {
        spiPull();
        if (packetInputBytesLeft > 0 || packetOutputBytesLeft > 0 || packetByteId == 0) {
            packetByteId++;
            spiPush(); // IRQHandler will be called again once the transmission is done
        } else { // there are no more bytes to send or receive
            packetByteId = 0;
            packetInputBytesLeft = 0;
            packetOutputBytesLeft = 0;
            PIN_REQN.turnOff(); // we turn off REQN after every packet
        }
        //SPI_Cmd(SPI1, DISABLE); // that's it, one packet at a time
    }
}
