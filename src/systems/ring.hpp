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
#include <stm32f10x_gpio.h>
#include <cstdbool>

#include "../core/progmisc.hpp"

static const int RING_BUTTON_COUNT = 2;
extern int lastPress[];

static Pin PIN_RING_BUTTON_1(GPIOA, GPIO_Pin_0, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, true);
static Pin PIN_RING_BUTTON_2(GPIOC, GPIO_Pin_1, GPIO_Speed_2MHz, GPIO_Mode_IPU, false);

void configureRing();
void resetButtons();
//void buttonRelease();
