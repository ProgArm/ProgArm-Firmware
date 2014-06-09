#include <stm32f10x_rcc.h>
#include "indicator.hpp"
#include "timing.hpp"

RCC_ClocksTypeDef RCC_ClockFreq; // XXX why here?
ErrorStatus HSEStartUpStatus; // XXX why here?

volatile int milliseconds = 0;

void setSysClockToHSE() { // TODO not really HSE yet
    SystemInit();
    //RCC_HSEConfig(RCC_HSE_ON); // Enable HSE

    //HSEStartUpStatus = RCC_WaitForHSEStartUp(); // Wait till HSE is ready

    //if (HSEStartUpStatus == SUCCESS) {
    RCC_HCLKConfig(RCC_SYSCLK_Div1); // HCLK = SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); // PCLK2 = HCLK
    RCC_PCLK1Config(RCC_HCLK_Div2); // PCLK1 = HCLK
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); // Select HSI as system clock source

    SystemCoreClockUpdate();

    //while (RCC_GetSYSCLKSource() != 0x04)
    //    ; // Wait till PLL is used as system clock source
    /*} else { // If HSE fails to start-up, the application will have wrong clock configuration.
     notifyAboutError(NO_HSE);
     while (1)
     // TODO perhaps we should ignore this unfortunate fact and continue?
     ;
     }*/
}

void setSysClockToPLL() {
    RCC_DeInit(); // RCC system reset(for debug purpose)

    RCC_HSEConfig(RCC_HSE_ON); // Enable HSE

    HSEStartUpStatus = RCC_WaitForHSEStartUp(); // Wait till HSE is ready

    if (HSEStartUpStatus == SUCCESS) {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // Enable Prefetch Buffer

        FLASH_SetLatency(FLASH_Latency_2); // Flash 2 wait state

        RCC_HCLKConfig(RCC_SYSCLK_Div1); // HCLK = SYSCLK
        RCC_PCLK2Config(RCC_HCLK_Div1); // PCLK2 = HCLK
        RCC_PCLK1Config(RCC_HCLK_Div2); //PCLK1 = HCLK/2

        //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // PLLCLK = 8MHz * 9 = 72 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_2); // PLLCLK = 8MHz * 16 = 128 MHz
        // The frequency has also been changed in system_stm32f10x

        RCC_PLLCmd(ENABLE); // Enable PLL

        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) { // Wait till PLL is ready
            ;
        }

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // Select PLL as system clock source

        while (RCC_GetSYSCLKSource() != 0x08) { // Wait till PLL is used as system clock source
            ;

        }

    }
}

extern "C" void SysTick_Handler(void) {
    milliseconds++;
    //if (milliseconds % 1000 == 0) {
    //LED_GPIO[LED_GREEN]->BRR = LED_PINS[LED_GREEN];
    //LED_GPIO[LED_GREEN]->BSRR = LED_PINS[LED_GREEN];
    //}
}

void configureTime() {
    setSysClockToHSE();
    SysTick_Config(8000);
}
