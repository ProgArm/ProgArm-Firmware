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
#include "i2c.hpp"

// TODO complete it

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
    I2C_GetAndSet(BQ24297_ADDRESS, 0x00, 3, 3, mVolts);
}

//int get_IINLIM() {
    // TODO
//}

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

void set_OTG_CONFIG(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 5, 1, set);
}

bool get_CHG_CONFIG() {
    return I2C_Get(BQ24297_ADDRESS, 0x01, 4, 1);
}

void set_CHG_CONFIG(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x01, 4, 1, set);
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

bool get_BCOLD() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 1, 1);
}

void set_BCOLD(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 1, 1, set);
}

bool get_FORCE_20PCT() {
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

int get_VREG() { // in mV
    return I2C_Get(BQ24297_ADDRESS, 0x03, 7, 4) * 16 + 3504;
}

void set_VREG(int mV) { // in mV
    I2C_GetAndSet(BQ24297_ADDRESS, 0x03, 7, 4, (mV - 3504) / 16);
}

bool get_BATLOWV() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 1, 1);
}

void set_BATLOWV(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 1, 1, set);
}

bool get_VRECHG() { // TODO not bool
    return I2C_Get(BQ24297_ADDRESS, 0x02, 1, 1);
}

void set_VRECHG(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 1, 1, set);
}

// REG05

bool get_EN_TERM() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 7, 1);
}

void set_EN_TERM(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 7, 1, set);
}

// TODO WATCHDOG

bool get_EN_TIMER() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 3, 1);
}

void set_EN_TIMER(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 3, 1, set);
}

// TODO CHG_TIMER

// REG06

int get_BOOSTV() { // in mV
    return I2C_Get(BQ24297_ADDRESS, 0x03, 7, 4) * 64 + 4550;
}

void set_BOOSTV(int mV) { // in mV
    I2C_GetAndSet(BQ24297_ADDRESS, 0x03, 7, 4, (mV - 4550) / 64);
}

// TODO BHOT

// TODO TREG

// REG07

bool get_DPDM_EN() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 7, 1);
}

void set_DPDM_EN(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 7, 1, set);
}

bool TMR2X_EN() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 6, 1);
}

void TMR2X_EN(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 6, 1, set);
}

bool get_BATFET_Disable() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 5, 1);
}

void set_BATFET_Disable(bool set) {
    I2C_GetAndSet(BQ24297_ADDRESS, 0x02, 5, 1, set);
}

// TODO INT_MASK

// REG08

// TODO VBUS_STAT

// TODO CHRG_STAT

bool get_DPM_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 3, 1);
}

bool get_PG_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 2, 1);
}

bool get_THERM_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 1, 1);
}

bool get_VSYS_STAT() {
    return I2C_Get(BQ24297_ADDRESS, 0x02, 0, 1);
}
