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
#include "Notification.hpp"
#include "notificationManager.hpp"
#include "indicator.hpp"
#include "timing.hpp"

Notification::Notification(uint16_t r, uint16_t g, uint16_t b, int priority, int duration) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
    this->priority = priority;
    this->duration = duration;
}

bool Notification::update() {
    if (curDuration < 0) // negative duration indicates indefinite notification
        return false;
    curDuration -= milliseconds - lastUpdate;
    lastUpdate = milliseconds;
    return curDuration < 0; // timed out
}

void Notification::turnOn() {
    turnedOn = true;
    curDuration = duration;
    chooseMostPrioritizedNotification();
}

void Notification::turnOff() {
    turnedOn = false;
    pause();
}

void Notification::toggle() {
    if (turnedOn)
        turnOff();
    else
        turnOn();
}

void Notification::play() {
    setLedValues(color[0], color[1], color[2]);
    lastUpdate = milliseconds;
}

void Notification::pause() {
    setLedValues(0, 0, 0); // XXX should we do that?
}
