//#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>
#include <misc.h>
#include <algorithm>

#include "ring.hpp"
#include "indicator.hpp"
#include "timing.hpp"
#include "connection.hpp"
#include "actions.hpp"

volatile bool pressed[RING_BUTTON_COUNT];
volatile bool wasPressed[RING_BUTTON_COUNT];
volatile int lastPress[RING_BUTTON_COUNT];

static const int DEBOUNCE_MILLISECONDS = 10;
static const int CLICK_SHORT_MILLISECONDS = 50;
static const int CLICK_LONG_MILLISECONDS = 280;
volatile uint16_t action = 0;
volatile int clicks = 0;

static const int CLICK_TIMEOUT_MILLISECONDS = 500;

void configureRing() {
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    gpio.GPIO_Pin = RING_BUTTON_PINS[0];
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = RING_BUTTON_PINS[1];
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &gpio);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Second button
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    for (int i = 0; i < RING_BUTTON_COUNT; i++) {
        pressed[i] = false;
        wasPressed[i] = false;
        lastPress[i] = 0;
    }
}

void resetButtons() {
    if (milliseconds - std::max(lastPress[0], lastPress[1]) < CLICK_TIMEOUT_MILLISECONDS)
        return; // no reset needed

    if (clicks != 0) {
        LED_GPIO[LED_RED]->BRR = LED_PINS[LED_RED];
        LED_GPIO[LED_RED]->BSRR = LED_PINS[LED_RED];
    }
    for (int i = 0; i < RING_BUTTON_COUNT; i++) {
        pressed[i] = false;
        wasPressed[i] = false;
        //lastPress[i]=0;
    }
    action = 0;
    clicks = 0;
}

void buttonRelease() { // TODO surely it is meant to work with two buttons only
    if (!wasPressed[0] && !wasPressed[1]) // this release was already processed.
        return;

    int delta1 = milliseconds - lastPress[0];
    int delta2 = milliseconds - lastPress[1];
    int minDelta = delta1 < delta2 ? delta1 : delta2;

    bool shortpress = minDelta < (CLICK_LONG_MILLISECONDS + CLICK_SHORT_MILLISECONDS) / 2;

    clicks++;

    if (shortpress) {
        if (wasPressed[0] && !wasPressed[1])
            action += 0;
        else if (!wasPressed[0] && wasPressed[1])
            action += 2;
        else if (wasPressed[0] && wasPressed[1])
            action += 4;
    } else { // long press
        if (wasPressed[0] && !wasPressed[1])
            action += 1;
        else if (!wasPressed[0] && wasPressed[1])
            action += 3;
        else if (wasPressed[0] && wasPressed[1])
            action += 5;
    }

    wasPressed[0] = false;
    wasPressed[1] = false;

    if (clicks == 2) {
        processAction(action);
        clientPut('L'); // TODO this can take a long time, we want to end interrupt call as fast as we can
        clientPut(action); // TODO
        action = 0;
        clicks = 0;
    } else
        action *= 6; // shift left in base 6
}

void processInterrupt(int buttonId) {
    int delta = milliseconds - lastPress[buttonId];
    if (milliseconds == 0)
        GPIO_WriteBit(LED_GPIO[0], LED_PINS[0], Bit_RESET);
    if (delta > DEBOUNCE_MILLISECONDS) {
        pressed[buttonId] = !pressed[buttonId];
        if (pressed[buttonId]) { // press
            wasPressed[buttonId] = true;
            GPIO_WriteBit(LED_GPIO[buttonId + 1], LED_PINS[buttonId + 1], Bit_RESET); // buttonId + 1 is a hack
            GPIO_WriteBit(LED_GPIO[buttonId + 1], LED_PINS[buttonId + 1], Bit_SET);
        } else { // release
            GPIO_WriteBit(LED_GPIO[buttonId + 1], LED_PINS[buttonId + 1], Bit_RESET);
            GPIO_WriteBit(LED_GPIO[buttonId + 1], LED_PINS[buttonId + 1], Bit_SET);
            buttonRelease();
        }
        lastPress[buttonId] = milliseconds;
    }
}

extern "C" void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
        processInterrupt(0);
    EXTI_ClearITPendingBit(EXTI_Line0);
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
        processInterrupt(1);
    EXTI_ClearITPendingBit(EXTI_Line12);
}
