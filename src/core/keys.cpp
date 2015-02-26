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

#include <stm32f10x.h>

#include "../input_codes.hpp"
#include "actions.hpp"
#include "connection.hpp"

namespace {
bool shiftPressed = false;
bool shiftHolded = false;
bool fnPressed = false;
bool fnHolded = false;
}

void processKey(u16 action) {
    if (action == INPUT_Shift) {
        if (shiftHolded) {
            shiftHolded = false;
            shiftPressed = false;
        } else {
            if (shiftPressed) // double press
                shiftHolded = true;
            shiftPressed = true;
        }
    } else if (shiftPressed) {
        action += 100; // shift keys are shifted by 100. (this might change in the future)
        if (!shiftHolded)
            shiftPressed = false;
    }
    processAction(action);
    printAction(action);
}
