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

#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <cstdbool>

#include "progmisc.hpp"

static const u32 BLUETOOTH_BAUD = 38400;
static Pin PIN_BLUETOOTH_POWER(GPIOC, GPIO_Pin_6, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, false);
static Pin PIN_BLUETOOTH_KEY(GPIOA, GPIO_Pin_8, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, true);

void configureConnection();

void processIncomingData();

void printAction(u8 action);
void print(const char* str);
void printPlain(const char* str);
void clientPut(u8 ch);
