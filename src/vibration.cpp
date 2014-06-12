#include "vibration.hpp"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

void configureVibration() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpio);

    TIM3->CCER |= (TIM_CCER_CC3E); // select used pins
    TIM3->CCMR2 |= (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2); // inverse PWM
    TIM3->CR1 |= TIM_CR1_CEN; // enable counter

    TIM3->CCR3 = 0; // turn it off
}

void setVibration(uint16_t amount) {
    TIM3->CCR3 = amount;
}
