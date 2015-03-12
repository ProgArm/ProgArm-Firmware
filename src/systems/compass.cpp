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

#include "compass.hpp"

#include <cstdbool>

namespace compass {

bool compasOutput = false;

void configure() {

}

void toggle() {
    compasOutput = !compasOutput;
    /*if (compasOutput)
     PIN_POWER_PERIPHERAL.turnOn();
     else
     PIN_POWER_PERIPHERAL.turnOff(); // XXX*/
}

void update() {
    /*char buffer[50];
     if (compasOutput) {
     s16 compass[3];
     //
     sprintf(buffer, "Compass: %d\t%d\t%d\n", compass[0], compass[1], compass[2]);
     printPlain(buffer);
     }*/
}

}
