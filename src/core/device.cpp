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

#include <stm32f10x_rcc.h>

#include "../systems/indicator.hpp"
#include "../systems/ring.hpp"
#include "../systems/vibration.hpp"
#include "bkp.hpp"
#include "connection.hpp"
#include "i2c.hpp"
#include "rtc.hpp"
#include "timing.hpp"
#include "wakeup.hpp"

void configureDevice() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB //
            | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); // We will need this for sure
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    configureTime();
    BKP_Setup(); // TODO rename these?
    RTC_Setup();
    I2C_Setup();
    //SPI_Setup();

    //PIN_POWER_MODE.init();
    //PIN_POWER_MODE.turnOn(); // we expect high current when bluetooth is on
    //PIN_POWER_PERIPHERAL.init();
    PIN_POWER_PORTS.init();
    PIN_POWER_PORTS.turnOn();
    //PIN_CHARGE.init();

    configureWakeups();
    configureLed();
    configureConnection();
    configureRing();
    configureVibration();
    //configureBeeper();

    //PIN_POWER_PERIPHERAL.turnOn();
    //PIN_POWER_PORTS.turnOn();

    /*volatile int i;
     for (i = 0; i < 100000; i++)
     ;
     //configureCompass();
     //configureAccelgyro();
     */
    volatile int i;
    for (i = 0; i < 100000; i++)
        ; // I2C initialization takes some time
}
