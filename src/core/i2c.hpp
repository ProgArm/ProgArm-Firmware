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

#include <stm32f10x.h>
#include <sys/types.h>

void I2C_Setup();
void I2C_Write(u8 address, u8 reg, u8 value);
u8 I2C_Receive(u8 Address, u8 Register);
uint I2C_ReceiveMany(u8 Address, u8 Register, int count);
u8 I2C_Get(u8 address, u8 reg, int msbIndex, int size);
u8 I2C_GetAndSet(u8 address, u8 reg, int msbIndex, int size, u8 newData);
