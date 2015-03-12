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

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_rcc.h>
#include <sys/types.h>

// TODO use interrupts to process I2C

#define I2C_SPEED 10000 // XXX

namespace i2c {

void setup() {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    I2C_Cmd(I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
    I2C_Init(I2C1, &I2C_InitStructure);
}

void write(u8 address, u8 reg, u8 value) {
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
        ;
    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(I2C1, reg);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_SendData(I2C1, value);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))
        ;
}

void readStart(u8 address, u8 reg) {
    I2C_AcknowledgeConfig(I2C1, ENABLE); // re-enable ACK bit in case it was disabled last call
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        ;
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
        ;
    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(I2C1, reg);
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE))
        ;
    I2C_GenerateSTART(I2C1, ENABLE); // Re-Start
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
        ;
    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
}

void readStop() {
    // enable NACK bit
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))
        ;
}

u8 receive(u8 address, u8 reg) {
    u8 out;
    readStart(address, reg);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    out = I2C_ReceiveData(I2C1);
    readStop();
    return out;
}

// reg should point to the first byte (usually MSB)
uint receiveMany(u8 address, u8 reg, int count) {
    uint out = 0;
    readStart(address, reg);
    for (int i = 0; i < count; i++) {
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
            ;
        if (i != 0)
            out <<= 8;
        out += I2C_ReceiveData(I2C1);
    }
    readStop();
    return out;
}

u8 get(u8 address, u8 reg, int msbIndex, int size) {
    u8 kindaMask = (u8) (((uint) 1 << size) - 1);
    u8 data = receive(address, reg);
    data >>= msbIndex - size + 1;
    data &= kindaMask;
    return data;
}

u8 getAndSet(u8 address, u8 reg, int msbIndex, int size, u8 newData) {
    int shift = msbIndex - size + 1;
    u8 kindaMask = (u8) (((uint) 1 << size) - 1);
    newData &= kindaMask; // protection // TODO assert?
    u8 data = receive(address, reg);
    data &= ~(kindaMask << shift); // reset existing
    data |= newData << shift;
    write(address, reg, data);
    return newData;
}

}
