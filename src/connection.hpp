#pragma once

#include <stdio.h>
#include <stdint.h>
#include "stm32f10x_gpio.h"

static const uint32_t BLUETOOTH_BAUD = 38400;
static const uint16_t BLUETOOTH_KEY = GPIO_Pin_3;

static const uint16_t POWER_BLUETOOTH = GPIO_Pin_15;

void configureConnection();
void bluetoothSwitchPower(bool turnOn);

void processIncomingData();

void print(const char* str);
void printPlain(const char* str);
void printSensor(uint8_t sensor, const char* str);
void clientPut(uint8_t ch);
uint8_t clientGet();
