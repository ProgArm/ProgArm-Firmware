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
#include "input_codes.hpp"
#include "indicator.hpp"
#include "actions.hpp"
#include "compass.hpp"

int currentAction = ACTION_NONE;
bool deviceActionsEnabled = true;
bool ignoreTable[36] = { }; // XXX bools are stored inefficiently

void changeState(uint8_t option) {
    switch (option) {
    case 0:
    case '0':
        deviceActionsEnabled = false;
        break;
    case 1:
    case '1':
        deviceActionsEnabled = true;
        break;
    case 2:
    case '2':
        deviceActionsEnabled = !deviceActionsEnabled;
        break;
    }
}

void processAction(int action) {
    // TODO check action range
    if (!deviceActionsEnabled)
        return;

    if (currentAction != ACTION_NONE) {
        switch (currentAction) {
        case ACTION_IGNORE:
            ignoreTable[action] = !ignoreTable[action];
            currentAction = ACTION_NONE;
            break;
        default:
            break;
        }
    } else {
        if (ignoreTable[action]) // this action should be ignored
            return;
        switch (action) {
        case INPUT_K:
            currentAction = ACTION_IGNORE;
            break;
        case INPUT_J:
            flashlightToggle();
            break;
        case INPUT_I:
            presenceToggle();
            break;
        case INPUT_O:
            compassToggle();
            break;
        default:
            break;
        }
    }
}
