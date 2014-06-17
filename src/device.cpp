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
#include "timing.hpp"
#include "connection.hpp"
#include "indicator.hpp"
#include "ring.hpp"
#include "vibration.hpp"
#include "compass.hpp"

void disableUnusedFeatures() {
    // JTAG pins are used for device peripherals
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
}

void configurePeripheralPower() {
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = POWER_PERIPHERALS;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpio);
}

void peripheralsSwitchPower(bool turnOn) {
    GPIO_WriteBit(GPIOB, POWER_PERIPHERALS, turnOn ? Bit_RESET : Bit_SET);
}

void configureDevice() {
    configureTime();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // We will need this for sure

    disableUnusedFeatures();
    configureLed();
    configurePeripheralPower();
    configureConnection();
    configureRing();
    configureVibration();

    peripheralsSwitchPower(true);
    bluetoothSwitchPower(true);

    volatile int i;
    for (i = 0; i < 100000; i++)
        ;
    //configureCompass();
}
