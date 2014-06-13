#include "lib/HMC5883L.h"
#include "compass.hpp"
#include "connection.hpp"
#include <stdio.h>

bool indicationEnabled = false;

void configureCompass() {
    HMC5883L_I2C_Init();
    HMC5883L_Initialize();
}

void compassToggle() {
    indicationEnabled = !indicationEnabled;
}

char buffer[50];
void updateCompass() {
    if (indicationEnabled) {
        int16_t compass[3];
        HMC5883L_GetHeading(compass);
        sprintf(buffer, "%d\t%d\t%d\n", compass[0], compass[1], compass[2]);
        printSensor('C', buffer);
    }
}
