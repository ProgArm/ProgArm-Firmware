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

static const uint32_t CLOCK_FREQUENCY = 8000000;

static Pin PIN_CHARGE(GPIOC, GPIO_Pin_3, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, false); // XXX it is meant to be input, not output
static Pin PIN_POWER_MODE(GPIOC, GPIO_Pin_5, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, false);
static Pin PIN_POWER_PERIPHERAL(GPIOC, GPIO_Pin_7, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, false);
static Pin PIN_POWER_PORTS(GPIOB, GPIO_Pin_1, GPIO_Speed_2MHz, GPIO_Mode_Out_OD, false);

void configureDevice();
