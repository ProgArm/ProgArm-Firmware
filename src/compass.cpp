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
#include "lib/HMC5883L.h"
#include "compass.hpp"
#include "connection.hpp"
#include <stdio.h>

bool indicationEnabled = false;

void configureCompass() {
    HMC5883L_I2C_Init();
    HMC5883L_Initialize();
}

void compassToggle() {
    indicationEnabled = !indicationEnabled;
}

char buffer[50];
void updateCompass() {
    if (indicationEnabled) {
        //int16_t compass[3];
        //HMC5883L_GetHeading(compass);
        //sprintf(buffer, "%d\t%d\t%d\n", compass[0], compass[1], compass[2]);
        //printSensor('C', buffer);
        //print(buffer);
    }
}
