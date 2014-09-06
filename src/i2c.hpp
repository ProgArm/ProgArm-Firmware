#pragma once

void I2C_Setup();
void I2C_Write(u8 address, u8 reg, u8 value);
u8 I2C_Receive(u8 Address, u8 Register);
int I2C_ReceiveMany(u8 Address, u8 Register, int count);
