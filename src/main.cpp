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
#include "device.hpp"
#include "ring.hpp"
#include "connection.hpp"
#include "indicator.hpp"
#include "stdio.h"
#include <stdlib.h>
#include "compass.hpp"
#include "stm32f10x_pwr.h"
#include "timing.hpp"

#include <misc.h>
#include <algorithm>

int main(void) {
    configureDevice();
    int count = 0;

    while (1) {
        resetButtons();
        processIncomingData();
        updateCompass();
        count++;

        if (milliseconds - std::max(lastPress[0], lastPress[1]) > 1000) {
            __WFI();
            //PWR_EnterSTANDBYMode();
            //PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        }
    }
}
