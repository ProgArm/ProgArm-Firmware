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
#include <stm32f10x.h>

class Notification {
public:
    u16 color[3];
    int priority;
    int duration;
    int curDuration = 0;bool turnedOn = false;

    Notification(u16 r, u16 g, u16 b, int priority = 0, int duration = -1); //
    bool update();
    void turnOn();
    void turnOff();
    void toggle();
    void pause();
    void play();
protected:
    int lastUpdate = 0;
};
