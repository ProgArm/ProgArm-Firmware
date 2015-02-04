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

#pragma once

#include <stm32f10x.h>
#include <sys/types.h>

const int BQ24297_ADDRESS = 0x6B;

bool get_EN_HIZ();
void set_EN_HIZ(bool set);
uint get_VINDPM();
void set_VINDPM(uint mVolts);
uint get_IINLIM();
void set_IINLIM(uint mA);

void registerReset();
void i2cWatchdogTimerReset();
bool get_OTG_CONFIG();
void set_OTG_CONFIG(bool enabled);
bool get_CHG_CONFIG();
void set_CHG_CONFIG(bool enable);
uint get_SYS_MIN();
void set_SYS_MIN(uint mVolts);
bool get_BOOST_LIM();
void set_BOOST_LIM(bool set);

uint get_ICHG();
void set_ICHG(uint mA);
bool get_BCOLD();
void set_BCOLD(bool set);
bool get_FORCE_20PCT();
void set_FORCE_20PCT(bool set);

uint get_IPRECHG();
void set_IPRECHG(uint mA);
uint get_ITERM();
void set_ITERM(uint mA);

uint get_VREG();
void set_VREG(uint mV);
bool get_BATLOWV();
void set_BATLOWV(bool set);
bool get_VRECHG();
void set_VRECHG(bool set);

bool get_EN_TERM();
void set_EN_TERM(bool set);
uint get_WATCHDOG();
void set_WATCHDOG(uint seconds);
bool get_EN_TIMER();
void set_EN_TIMER(bool enable);
bool get_CHG_TIMER();
void set_CHG_TIMER(bool enable);

uint get_BOOSTV();
void set_BOOSTV(uint mV);
u8 get_BHOT();
void set_BHOT(u8 value);
uint get_TREG();
void set_TREG(uint temp);

bool get_DPDM_EN();
void set_DPDM_EN(bool set);
bool TMR2X_EN();
void TMR2X_EN(bool set);
bool get_BATFET_Disable();
void set_BATFET_Disable(bool set);
bool get_INT_CHRG_FAULT();
void set_INT_CHRG_FAULT(bool intEnabled);
bool get_INT_BAT_FAULT();
void set_INT_BAT_FAULT(bool intEnabled);

uint get_VBUS_STAT();
void set_VBUS_STAT(uint value);
uint get_CHRG_STAT();
void set_CHRG_STAT(uint value);
bool get_DPM_STAT();
bool get_PG_STAT();
bool get_THERM_STAT();
bool get_VSYS_STAT();
