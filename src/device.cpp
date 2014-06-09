#include <stm32f10x_rcc.h>

#include "device.hpp"
#include "timing.hpp"
#include "connection.hpp"
#include "indicator.hpp"
#include "ring.hpp"
#include "accelgyro/MPU6050.hpp"
#include "compass/HMC5883L.hpp"

void disableUnusedFeatures() {
    // JTAG pins are used for device peripherals
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    //DELETEME
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio);

    GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
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

void Delay(volatile uint32_t nCount) {
    for (; nCount != 0; nCount--)
        ;
}

void configureDevice() {
    configureTime();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // We will need this for sure

    disableUnusedFeatures();
    configureConnection();
    configureLed();
    configureRing();

    peripheralsSwitchPower(true);
    bluetoothSwitchPower(true);

    Delay(0x1FFFF);

    MPU6050_I2C_Init();
    MPU6050_Initialize();
    if (MPU6050_TestConnection()) {
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
    } else {
    }

    HMC5883L_I2C_Init();
    HMC5883L_Initialize();
    if (HMC5883L_TestConnection()) {
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
    } else {
    }
    /*
     int16_t MagneticHeading[3] = { 0 };

     //int16_t accelgyro[6] = { 0 };
     int count = 0;
     char buffer[50];

     RCC_ClocksTypeDef RCC_ClockFreq;
     RCC_GetClocksFreq(&RCC_ClockFreq);

     while (1) {
     Delay(500);
     if (count % 2 == 0) {
     //GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
     sprintf(buffer, "SYS:%d H:%d, P1:%d, P2:%d, ADC:%d\r\n", RCC_ClockFreq.SYSCLK_Frequency, //
     RCC_ClockFreq.HCLK_Frequency,       // AHB
     RCC_ClockFreq.PCLK1_Frequency,  // APB1
     RCC_ClockFreq.PCLK2_Frequency,  // APB2
     RCC_ClockFreq.ADCCLK_Frequency);
     printPlain(buffer);
     //MPU6050_GetDeviceID();
     //MPU6050_GetRawAccelGyro(accelgyro);
     //sprintf(buffer, "%d\t%d\t%d | %d\t%d\t%d", accelgyro[0], accelgyro[1], accelgyro[2], accelgyro[3], accelgyro[4], accelgyro[5]);
     HMC5883L_GetHeading(MagneticHeading);
     sprintf(buffer, "%d\t%d\t%d\n", MagneticHeading[0], MagneticHeading[1], MagneticHeading[2]);
     printPlain(buffer);
     //sprintf(buffer, "Device id: %d; ", MPU6050_GetDeviceID());
     //MPU6050_GetSleepModeStatus();
     } //else
     //  GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
     count++;
     }*/

    /*
     configurePeripheralPower();
     peripheralsSwitchPower(true);
     volatile int i = 0;
     for (int i = 0; i < 200000; ++i)
     asm("nop");

     //peripheralsSwitchPower(false);

     MPU6050_I2C_Init();
     //MPU6050_Initialize();
     if (MPU6050_TestConnection()) {
     GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
     } else {
     }
     //    int16_t accelgyro[6];
     //    MPU6050_GetRawAccelGyro(accelgyro);

     //flashlightToggle();
     //flashlightToggle();
     //    flashlightToggle();

     /*HMC5883L_I2C_Init();
     HMC5883L_Initialize();
     if (HMC5883L_TestConnection()) {
     // connection success
     } else {
     // connection failed
     }
     flashlightToggle();*/
    //bluetoothSwitchPower(true);
}
