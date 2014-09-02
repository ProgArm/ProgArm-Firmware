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
#include "notificationManager.hpp"
#include <cstddef>
#include <stm32f10x_tim.h>
#include "wakeup.hpp"

Notification* activeNotification;
std::vector<Notification*> notifications;

void addNotification(Notification* newNotification) {
    if (notifications.size() == 0) // put dummy notification so that we always have something to fall back to
        notifications.push_back(new Notification(0, 0, 0, -10000));
    notifications.push_back(newNotification);
    chooseMostPrioritizedNotification();
}

void updateNotification() {
    if (activeNotification != NULL)
        if (!activeNotification->turnedOn || activeNotification->update()) {
            activeNotification->turnOff();
            chooseMostPrioritizedNotification();
        }
}

void chooseMostPrioritizedNotification() {
    if (activeNotification != NULL && activeNotification->turnedOn)
        activeNotification->pause();
    activeNotification = NULL;
    for (auto &curNotification : notifications) // XXX not the most efficient way, but it is good enough for now
        if (curNotification->turnedOn)
            if (activeNotification == NULL || activeNotification->priority <= curNotification->priority) {
                activeNotification = curNotification;
                addWakeUp(activeNotification->curDuration);
            }
    if (activeNotification != NULL)
        activeNotification->play();
}
