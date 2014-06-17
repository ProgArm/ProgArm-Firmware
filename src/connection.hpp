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
#pragma once

#include <stdio.h>
#include <stdint.h>
#include "stm32f10x_gpio.h"

static const uint32_t BLUETOOTH_BAUD = 38400;
static const uint16_t BLUETOOTH_KEY = GPIO_Pin_3;

static const uint16_t POWER_BLUETOOTH = GPIO_Pin_15;

void configureConnection();
void bluetoothSwitchPower(bool turnOn);
void bluetoothTogglePower();

void processIncomingData();

void printAction(uint8_t action);
void print(const char* str);
void printPlain(const char* str);
void printSensor(uint8_t sensor, const char* str);
void clientPut(uint8_t ch);
uint8_t clientGet();
