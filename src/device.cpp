#include "device.hpp"
#include "timing.hpp"
#include "connection.hpp"
#include "indicator.hpp"
#include "ring.hpp"
#include "vibration.h"

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
    configureConnection();
    configureLed();
    configureRing();
    configureVibration();

    peripheralsSwitchPower(true);
    bluetoothSwitchPower(true);
}
