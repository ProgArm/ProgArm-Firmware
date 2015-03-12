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

#include "accelgyro.hpp"

#include <cstdbool>


// TODO this file is currently obsolete

namespace accelgyro {

bool accelgyroOutput = false;

void configure() {
    //MPU6050_I2C_Init();
    //MPU6050_Initialize();
}

void toggle() {
    accelgyroOutput = !accelgyroOutput;
    /*if (accelgyroOutput)
     PIN_POWER_PERIPHERAL.turnOn();
     else
     PIN_POWER_PERIPHERAL.turnOff(); // XXX*/
}

void update() {
    //char buffer[50];
    /*if (accelgyroOutput) {
     s16 accelgyro[6];
     //MPU6050_GetRawAccelGyro(accelgyro);
     indicator::setLedValues(abs(accelgyro[0]) / 200, abs(accelgyro[1]) / 200, abs(accelgyro[2]) / 200);
     //setLedValues(abs(accelgyro[3]) / 100, abs(accelgyro[4]) / 100, abs(accelgyro[5]) / 100);
     sprintf(buffer, "AccelGyro: %d\t%d\t%d\t%d\t%d\t%d\n", accelgyro[0], accelgyro[1], accelgyro[2],
     accelgyro[3], accelgyro[4], accelgyro[5]);
     printPlain(buffer);
     }*/
}

}
