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

#include "components/nrf8001.hpp"
#include "core/device.hpp"
#include "core/notificationManager.hpp"
#include "core/wakeup.hpp"
#include "systems/ring.hpp"

int main(void) {
    device::configure();
    int count = 0;

    int lastupdate = 0;
    while (1) {
        count++;
        ring::resetButtons();

        notificationManager::update();

        nrf8001::processIncomingPacket();

        wakeup::setWakeTimer();
        //__WFI(); // TODO LED PWM wakes us up? What the hell?
        //PWR_EnterSTANDBYMode();
        //PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    }
}
