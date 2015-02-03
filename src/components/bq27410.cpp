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

#include "bq27410.hpp"

#include "../core/i2c.hpp"

int getControl() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x01, 2);
}

int getTemperature() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x03, 2);
}

int getVoltage() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x05, 2);
}

int getFlags() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x07, 2);
}

int getNominalAvailableCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x09, 2);
}

int getFullAvailableCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0B, 2);
}

int getRemainingCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0D, 2);
}

int getFullChargeCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0F, 2);
}

int getAverageCurrent() {
    return (s16) I2C_ReceiveMany(BQ27410_ADDRESS, 0x11, 2);
}

int getStandbyCurrent() {
    return (s16) I2C_ReceiveMany(BQ27410_ADDRESS, 0x13, 2);
}

int getMaxLoadCurrent() {
    return (s16) I2C_ReceiveMany(BQ27410_ADDRESS, 0x15, 2);
}

int getAvailableEnergy() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x17, 2);
}

int getAveragePower() {
    return (s16) I2C_ReceiveMany(BQ27410_ADDRESS, 0x19, 2);
}

int getStateOfCharge() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x1D, 2);
}

int getIntTemperature() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x1F, 2);
}

int getStateofHealth() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x21, 2);
}
