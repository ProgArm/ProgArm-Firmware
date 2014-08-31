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
#include <stdbool.h>
#include <stdint.h>

class Notification {
public:
    uint16_t color[3];
    int priority;
    int duration;
    int curDuration = 0;bool turnedOn = false;

    Notification(uint16_t r, uint16_t g, uint16_t b, int priority = 0, int duration = -1); //
    bool update();
    void turnOn();
    void turnOff();
    void toggle();
    void pause();
    void play();
protected:
    int lastUpdate = 0;
};
