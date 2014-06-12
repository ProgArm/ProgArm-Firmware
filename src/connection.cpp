#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include "actions.hpp"
#include "connection.hpp"

void configureConnection() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef gpio;

    gpio.GPIO_Pin = POWER_BLUETOOTH;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_StructInit(&gpio);
    // Rx
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    // Tx
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = BLUETOOTH_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void printPlain(const char* str) {
    print("T");
    print(str);
    clientPut(0);
}

void printSensor(uint8_t sensor, const char* str) {
    print("S");
    clientPut(sensor);
    print(str);
    clientPut(0);
}

void print(const char* str) {
    for (unsigned int i = 0; str[i]; i++) {
        while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE))
            ;
        USART_SendData(USART1, str[i]);
    }
}

void clientPut(uint8_t ch) {
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE))
        // XXX before or after?
        ;
    USART_SendData(USART1, (uint8_t) ch);
    //Loop until the end of transmission
}

uint8_t clientGet() {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        ;
    return (uint8_t) USART_ReceiveData(USART1);
}

void bluetoothSwitchPower(bool turnOn) {
    GPIO_WriteBit(GPIOA, POWER_BLUETOOTH, turnOn ? Bit_RESET : Bit_SET);
}

void processIncomingData() { // TODO use interrupts to process data
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        return; // no data to read

    uint8_t received = (uint8_t) USART_ReceiveData(USART1);
    switch (received) {
    case 'p':
        clientPut('P');
        break;
    case 'E':
        changeState(clientGet()); // XXX what if client disconnects exactly at this moment?
        break;
    default:
        char buffer[60]; // XXX use some existining buffer instead?
        sprintf(buffer, "Error: Skipping unexpected byte: %d\n", received);
        printPlain(buffer);
        break;
    }
}
