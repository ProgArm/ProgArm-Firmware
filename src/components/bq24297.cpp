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

#include "../core/i2c.hpp"

// REG00

bool get_EN_HIZ() {
    return i2c::get(BQ24297_ADDRESS, 0x00, 7, 1);
}

void set_EN_HIZ(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x00, 7, 1, set);
}

uint get_VINDPM() {
    return 3880 + 80 * (uint) i2c::get(BQ24297_ADDRESS, 0x00, 6, 4); // in mV
}

void set_VINDPM(uint mVolts) { // mVolts should be in multiples of 80
    mVolts -= 3880; // offset
    mVolts /= 80;
    i2c::getAndSet(BQ24297_ADDRESS, 0x00, 6, 4, mVolts); // TODO add checks
}

uint get_IINLIM() { // in mA
    u8 data = i2c::get(BQ24297_ADDRESS, 0x00, 2, 3);
    switch (data) {
    case 0:
        return 100;
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

void set_IINLIM(uint mA) {
    if (mA >= 3000)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 7);
    else if (mA >= 2000)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 6);
    else if (mA >= 1500)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 5);
    else if (mA >= 1000)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 4);
    else if (mA >= 900)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 3);
    else if (mA >= 500)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 2);
    else if (mA >= 150)
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 1);
    else
        i2c::getAndSet(BQ24297_ADDRESS, 0x00, 2, 3, 0);

}

// REG01

void registerReset() {
    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 7, 1, 1);
}

void i2cWatchdogTimerReset() {
    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 6, 1, 1);
}

bool get_OTG_CONFIG() {
    return i2c::get(BQ24297_ADDRESS, 0x01, 5, 1);
}

void set_OTG_CONFIG(bool enabled) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 5, 1, enabled);
}

bool get_CHG_CONFIG() {
    return i2c::get(BQ24297_ADDRESS, 0x01, 4, 1);
}

// true if charge is enabled. OTG_CONFIG would override this
void set_CHG_CONFIG(bool enable) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 4, 1, enable);
}

uint get_SYS_MIN() {
    u8 data = i2c::get(BQ24297_ADDRESS, 0x01, 3, 3);
    uint mV = 3000;
    if ((data & 0b100) > 0)
        mV += 400;
    if ((data & 0b010) > 0)
        mV += 200;
    if ((data & 0b001) > 0)
        mV += 100;
    return mV;
}

void set_SYS_MIN(uint mVolts) {
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

    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 3, 3, data);
}

bool get_BOOST_LIM() {
    return i2c::get(BQ24297_ADDRESS, 0x01, 0, 1);
}

void set_BOOST_LIM(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x01, 0, 1, set);
}

// REG02

uint get_ICHG() { // in mA
    return i2c::get(BQ24297_ADDRESS, 0x02, 7, 6) * 64 + 512;
}

void set_ICHG(uint mA) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x02, 7, 6, (mA - 512) / 64); // TODO add checks
}

bool get_BCOLD() { // TODO
    return i2c::get(BQ24297_ADDRESS, 0x02, 1, 1);
}

void set_BCOLD(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x02, 1, 1, set);
}

bool get_FORCE_20PCT() { // TODO
    return i2c::get(BQ24297_ADDRESS, 0x02, 0, 1);
}

void set_FORCE_20PCT(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x02, 0, 1, set);
}

// REG03

uint get_IPRECHG() { // in mA
    return i2c::get(BQ24297_ADDRESS, 0x03, 7, 4) * 128 + 128;
}

void set_IPRECHG(uint mA) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x03, 7, 4, (mA - 128) / 128);
}

uint get_ITERM() { // in mA
    return i2c::get(BQ24297_ADDRESS, 0x03, 3, 4) * 128 + 128;
}

void set_ITERM(uint mA) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x03, 3, 4, (mA - 128) / 128);
}

// REG04

// Charge Voltage Limit (in mV)
uint get_VREG() {
    return i2c::get(BQ24297_ADDRESS, 0x04, 7, 4) * 16 + 3504;
}

// Charge Voltage Limit (in mV)
void set_VREG(uint mV) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x04, 7, 4, (mV - 3504) / 16);
}

// false - 2.8 V, true - 3.0 V
bool get_BATLOWV() {
    return i2c::get(BQ24297_ADDRESS, 0x04, 1, 1);
}

// false - 2.8 V, true - 3.0 V
void set_BATLOWV(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x04, 1, 1, set);
}

// Battery Recharge Threshold (below battery regulation voltage). false - 100 mV, true - 300 mV
bool get_VRECHG() {
    return i2c::get(BQ24297_ADDRESS, 0x04, 1, 1);
}

// Battery Recharge Threshold (below battery regulation voltage). false - 100 mV, true - 300 mV
void set_VRECHG(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x04, 1, 1, set);
}

// REG05

bool get_EN_TERM() {
    return i2c::get(BQ24297_ADDRESS, 0x05, 7, 1);
}

void set_EN_TERM(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x05, 7, 1, set);
}

uint get_WATCHDOG() {
    u8 data = i2c::get(BQ24297_ADDRESS, 0x05, 5, 1);
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

void set_WATCHDOG(uint seconds) {
    if (seconds == 0)
        i2c::getAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 0);
    else if (seconds <= 40)
        i2c::getAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 1);
    else if (seconds <= 80)
        i2c::getAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 2);
    else
        // if (seconds <= 160)
        i2c::getAndSet(BQ24297_ADDRESS, 0x05, 7, 1, 3);
}

bool get_EN_TIMER() {
    return i2c::get(BQ24297_ADDRESS, 0x05, 3, 1);
}

void set_EN_TIMER(bool enable) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x05, 3, 1, enable);
}

bool get_CHG_TIMER() {
    return i2c::get(BQ24297_ADDRESS, 0x05, 3, 1);
}

void set_CHG_TIMER(bool enable) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x05, 3, 1, enable);
}

// REG06

uint get_BOOSTV() { // in mV
    return i2c::get(BQ24297_ADDRESS, 0x06, 7, 4) * 64 + 4550;
}

void set_BOOSTV(uint mV) { // in mV
    i2c::getAndSet(BQ24297_ADDRESS, 0x06, 7, 4, (mV - 4550) / 64);
}

u8 get_BHOT() {
    return i2c::get(BQ24297_ADDRESS, 0x06, 3, 2);
}

void set_BHOT(u8 value) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x06, 3, 2, value);
}

uint get_TREG() {
    return i2c::get(BQ24297_ADDRESS, 0x06, 1, 2) * 20 + 60;
}

void set_TREG(uint temp) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x06, 1, 2, (temp - 60) / 20);
}

// REG07

bool get_DPDM_EN() {
    return i2c::get(BQ24297_ADDRESS, 0x07, 7, 1);
}

void set_DPDM_EN(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x07, 7, 1, set);
}

bool get_TMR2X_EN() {
    return i2c::get(BQ24297_ADDRESS, 0x07, 6, 1);
}

void set_TMR2X_EN(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x07, 6, 1, set);
}

bool get_BATFET_Disable() {
    return i2c::get(BQ24297_ADDRESS, 0x07, 5, 1);
}

void set_BATFET_Disable(bool set) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x07, 5, 1, set);
}

bool get_INT_CHRG_FAULT() {
    return i2c::get(BQ24297_ADDRESS, 0x07, 1, 1);
}

void set_INT_CHRG_FAULT(bool intEnabled) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x07, 1, 1, intEnabled);
}

bool get_INT_BAT_FAULT() {
    return i2c::get(BQ24297_ADDRESS, 0x07, 0, 1);
}

void set_INT_BAT_FAULT(bool intEnabled) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x07, 0, 1, intEnabled);
}

// REG08

// XXX 0 - Unknown (no input, or DPDM detection incomplete), 1 - USB host, 2 - Adapter port, 3 - OTG
uint get_VBUS_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 7, 2);
}

// XXX 0 - Unknown (no input, or DPDM detection incomplete), 1 - USB host, 2 - Adapter port, 3 - OTG
void set_VBUS_STAT(uint value) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x08, 7, 2, value);
}

// XXX 0 - Not Charging, 1 - Pre-charge (<V_BATLOWV), 2 - Fast Charging, 3 - Charge Termination Done
uint get_CHRG_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 5, 2);
}

// XXX 0 - Not Charging, 1 - Pre-charge (<V_BATLOWV), 2 - Fast Charging, 3 - Charge Termination Done
void set_CHRG_STAT(uint value) {
    i2c::getAndSet(BQ24297_ADDRESS, 0x08, 5, 2, value);
}

bool get_DPM_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 3, 1);
}

bool get_PG_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 2, 1);
}

bool get_THERM_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 1, 1);
}

bool get_VSYS_STAT() {
    return i2c::get(BQ24297_ADDRESS, 0x08, 0, 1);
}
