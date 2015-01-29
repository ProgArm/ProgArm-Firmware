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

#include "bq24297.hpp"

#include <stm32f10x.h>

#include "../core/i2c.hpp"

// REG00

bool get_EN_HIZ() {
    return I2C_Get(BQ24297_ADDRESS, 0x00, 7, 1);
}

void set_EN_HIZ(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 7, 1, set);
}

int get_VINDPM() {
    return 3880 + 80 * (int) I2C_Get(BQ24297_ADDRESS, 0x00, 6, 4); // in mV
}

void set_VINDPM(int mVolts) { // mVolts should be in multiples of 80
    mVolts -= 3880; // offset
    mVolts /= 80;
    I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 6, 4, mVolts);
}

int get_IINLIM() {
    u8 data = I2C_Get(BQ24297_ADDRESS, 0x00, 2, 3);
    switch (data) {
    case 0:
        return 100; // in mA
    case 1:
        return 150;
    case 2:
        return 500;
    case 3:
        return 900;
    case 4:
        return 1000;
    case 5:
        return 1500;
    case 6:
        return 2000;
    case 7:
        return 3000;
    default:
        // should not happen
        return 0;
    }
}

void set_IINLIM(int mA) {
    if (mA >= 3000)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 7);
    else if (mA >= 2000)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 6);
    else if (mA >= 1500)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 5);
    else if (mA >= 1000)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 4);
    else if (mA >= 900)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 3);
    else if (mA >= 500)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 2);
    else if (mA >= 150)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 1);
    else
        I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 0);

}

// REG01

void registerReset() {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 7, 1, 1);
}

void i2cWatchdogTimerReset() {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 6, 1, 1);
}

bool get_OTG_CONFIG() {
    return I2C_Get(BQ24297_ADDRESS, 0x01, 5, 1);
}

void set_OTG_CONFIG(bool enabled) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 5, 1, enabled);
}

bool get_CHG_CONFIG() {
    return I2C_Get(BQ24297_ADDRESS, 0x01, 4, 1);
}

void set_CHG_CONFIG(bool enable) { // true if charge is enabled. OTG_CONFIG would override this
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 4, 1, enable);
}

int get_SYS_MIN() {
    u8 data = I2C_Get(BQ24297_ADDRESS, 0x01, 3, 3);
    int mV = 3000;
    if ((data & 0b100) > 0)
        mV += 400;
    if ((data & 0b010) > 0)
        mV += 200;
    if ((data & 0b001) > 0)
        mV += 100;
    return mV;
}

void set_SYS_MIN(int mVolts) {
    mVolts -= 3000;
    u8 data = 0;
    if (mVolts > 400) {
        data += 0b100;
        mVolts -= 400;
    }
    if (mVolts > 200) {
        data += 0b100;
        mVolts -= 200;
    }
    if (mVolts > 100)
        data += 0b100;

    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 3, 3, data);
}

bool get_BOOST_LIM() {
    return I2C_Get(BQ24297_ADDRESS, 0x01, 0, 1);
}

void set_BOOST_LIM(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 0, 1, set);
}

// REG02

int get_ICHG() { // in mA
    return I2C_Get(BQ24297_ADDRESS, 0x02, 7, 6) * 64 + 512;
}

void set_ICHG(int mA) { // in mA
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 7, 6, (mA - 512) / 64);
}

bool get_BCOLD() { // TODO
    return I2C_Get(BQ24297_ADDRESS, 0x02, 1, 1);
}

void set_BCOLD(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 1, 1, set);
}

bool get_FORCE_20PCT() { // TODO
    return I2C_Get(BQ24297_ADDRESS, 0x02, 0, 1);
}

void set_FORCE_20PCT(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 0, 1, set);
}

// REG03

int get_IPRECHG() { // in mA
    return I2C_Get(BQ24297_ADDRESS, 0x03, 7, 4) * 128 + 128;
}

void set_IPRECHG(int mA) { // in mA
    I2C_GetAndSet(BQ24297_ADDRESS, 0x03, 7, 4, (mA - 128) / 128);
}

int get_ITERM() { // in mA
    return I2C_Get(BQ24297_ADDRESS, 0x03, 3, 4) * 128 + 128;
}

void set_ITERM(int mA) { // in mA
    I2C_GetAndSet(BQ24297_ADDRESS, 0x03, 3, 4, (mA - 128) / 128);
}

// REG04

int get_VREG() { // Charge Voltage Limit (in mV)
    return I2C_Get(BQ24297_ADDRESS, 0x04, 7, 4) * 16 + 3504;
}

void set_VREG(int mV) { // Charge Voltage Limit (in mV)
    I2C_GetAndSet(BQ24297_ADDRESS, 0x04, 7, 4, (mV - 3504) / 16);
}

bool get_BATLOWV() { // false - 2.8 V, true - 3.0 V
    return I2C_Get(BQ24297_ADDRESS, 0x04, 1, 1);
}

void set_BATLOWV(bool set) { // false - 2.8 V, true - 3.0 V
    I2C_GetAndSet(BQ24297_ADDRESS, 0x04, 1, 1, set);
}

bool get_VRECHG() { // Battery Recharge Threshold (below battery regulation voltage). false - 100 mV, true - 300 mV
    return I2C_Get(BQ24297_ADDRESS, 0x04, 1, 1);
}

void set_VRECHG(bool set) { // Battery Recharge Threshold (below battery regulation voltage). false - 100 mV, true - 300 mV
    I2C_GetAndSet(BQ24297_ADDRESS, 0x04, 1, 1, set);
}

// REG05

bool get_EN_TERM() {
    return I2C_Get(BQ24297_ADDRESS, 0x05, 7, 1);
}

void set_EN_TERM(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 7, 1, set);
}

int get_WATCHDOG() {
    u8 data = I2C_Get(BQ24297_ADDRESS, 0x05, 5, 1);
    switch (data) {
    case 0:
        return 0;
    case 1:
        return 40;
    case 2:
        return 80;
    case 3:
        return 160;
    default:
        // Should not happen
        return 0;
    }
}

void set_WATCHDOG(int seconds) {
    if (seconds == 0)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 0);
    else if (seconds <= 40)
        I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 1);
    else if (seconds <= 80)
            I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 2);
    else // if (seconds <= 160)
            I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 3);
}

bool get_EN_TIMER() {
    return I2C_Get(BQ24297_ADDRESS, 0x05, 3, 1);
}

void set_EN_TIMER(bool enable) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 3, 1, enable);
}

bool get_CHG_TIMER() {
    return I2C_Get(BQ24297_ADDRESS, 0x05, 3, 1);
}

void set_CHG_TIMER(bool enable) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x05, 3, 1, enable);
}

// REG06

int get_BOOSTV() { // in mV
    return I2C_Get(BQ24297_ADDRESS, 0x06, 7, 4) * 64 + 4550;
}

void set_BOOSTV(int mV) { // in mV
    I2C_GetAndSet(BQ24297_ADDRESS, 0x06, 7, 4, (mV - 4550) / 64);
}

// TODO BHOT

// TODO TREG

// REG07

bool get_DPDM_EN() {
    return I2C_Get(BQ24297_ADDRESS, 0x07, 7, 1);
}

void set_DPDM_EN(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x07, 7, 1, set);
}

bool TMR2X_EN() {
    return I2C_Get(BQ24297_ADDRESS, 0x07, 6, 1);
}

void TMR2X_EN(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x07, 6, 1, set);
}

bool get_BATFET_Disable() {
    return I2C_Get(BQ24297_ADDRESS, 0x07, 5, 1);
}

void set_BATFET_Disable(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x07, 5, 1, set);
}

// TODO INT_MASK

// REG08

// TODO VBUS_STAT

// TODO CHRG_STAT

bool get_DPM_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x08, 3, 1);
}

bool get_PG_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x08, 2, 1);
}

bool get_THERM_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x08, 1, 1);
}

bool get_VSYS_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x08, 0, 1);
}
