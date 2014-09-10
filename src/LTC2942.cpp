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
#include <stm32f10x_i2c.h>
#include "i2c.hpp"

#define LTC2942_ADDRESS7 0b1100100
#define LTC2942_REG_STATUS                      0x00 // R
#define LTC2942_REG_CONTROL                     0x01 // R/W
#define LTC2942_REG_ACCUMULATED_CHARGE_MSB      0x02 // R/W
#define LTC2942_REG_ACCUMULATED_CHARGE_LSB      0x03 // R/W
#define LTC2942_REG_CHARGE_THRESHOLD_HIGH_MSB   0x04 // R/W
#define LTC2942_REG_CHARGE_THRESHOLD_HIGH_LSB   0x05 // R/W
#define LTC2942_REG_CHARGE_THRESHOLD_LOW_MSB    0x06 // R/W
#define LTC2942_REG_CHARGE_THRESHOLD_LOW_LSB    0x07 // R/W
#define LTC2942_REG_VOLTAGE_MSB                 0x08 // R
#define LTC2942_REG_VOLTAGE_LSB                 0x09 // R
#define LTC2942_REG_VOLTAGE_THRESHOLD_HIGH      0x0A // R/W
#define LTC2942_REG_VOLTAGE_THRESHOLD_LOW       0x0B // R/W
#define LTC2942_REG_TEMPERATURE_MSB             0x0C // R
#define LTC2942_REG_TEMPERATURE_LSB             0x0D // R
#define LTC2942_REG_TEMPERATURE_THRESHOLD_HIGH  0x0E // R/W
#define LTC2942_REG_TEMPERATURE_THRESHOLD_LOW   0x0F // R/W

void LTC2942_Configure() {
    // Defaults: ADC Mode [00], Prescaler [111], AL/CC Configure [10], Shutdown [0]
    I2C_Write(LTC2942_ADDRESS7, LTC2942_REG_CONTROL, 0b11101100); // temperature and voltage will be available after 10 ms
}

u16 LTC2942_GetBatteryCharge() {
    return I2C_ReceiveMany(LTC2942_ADDRESS7, LTC2942_REG_ACCUMULATED_CHARGE_MSB, 2);
}

u16 LTC2942_GetBatteryVoltage() {
    return I2C_ReceiveMany(LTC2942_ADDRESS7, LTC2942_REG_VOLTAGE_MSB, 2);
}

u16 LTC2942_GetBatteryTemperature() {
    return I2C_ReceiveMany(LTC2942_ADDRESS7, LTC2942_REG_TEMPERATURE_MSB, 2);
}

void LTC2942_ResetCharge() {
    // Defaults: ADC Mode [00], Prescaler [111], AL/CC Configure [10], Shutdown [0]
    I2C_Write(LTC2942_ADDRESS7, LTC2942_REG_CONTROL, 0b00101101);
    I2C_Write(LTC2942_ADDRESS7, LTC2942_REG_ACCUMULATED_CHARGE_MSB, 0xEE); // EEF8 with prescaler=32 is about 1300 mAh
    I2C_Write(LTC2942_ADDRESS7, LTC2942_REG_ACCUMULATED_CHARGE_LSB, 0xF8);
    LTC2942_Configure();
}
