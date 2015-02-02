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
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x00, 2);
}

int getTemperature() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x02, 2);
}

int getVoltage() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x04, 2);
}

int getFlags() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x06, 2);
}

int getNominalAvailableCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x08, 2);
}

int getFullAvailableCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0A, 2);
}

int getRemainingCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0C, 2);
}

int getFullChargeCapacity() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x0E, 2);
}

int getAverageCurrent() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x10, 2);
}

int getStandbyCurrent() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x12, 2);
}

int getMaxLoadCurrent() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x14, 2);
}

int getAvailableEnergy() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x16, 2);
}

int getAveragePower() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x18, 2);
}

int getStateOfCharge() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x1C, 2);
}

int getIntTemperature() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x1E, 2);
}

int getStateofHealth() {
    return I2C_ReceiveMany(BQ27410_ADDRESS, 0x20, 2);
}
