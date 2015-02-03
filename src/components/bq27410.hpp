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

const int BQ27410_ADDRESS = 0x55;

int getControl();
int getTemperature();
int getVoltage();
int getFlags();
int getNominalAvailableCapacity();
int getFullAvailableCapacity();
int getRemainingCapacity();
int getFullChargeCapacity();
int getAverageCurrent();
int getStandbyCurrent();
int getMaxLoadCurrent();
int getAvailableEnergy();
int getAveragePower();
int getStateOfCharge();
int getIntTemperature();
int getStateofHealth();
