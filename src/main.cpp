#include "device.hpp"
#include "ring.hpp"
#include "stdio.h"
#include "connection.hpp"

#include "accelgyro/MPU6050.hpp"
#include "compass/HMC5883L.hpp"

#include "indicator.hpp"

#include <stdlib.h>

int main(void) {
    configureDevice();

    int count = 0;

    char buffer[50];
    int16_t compass[3];
    int16_t accelgyro[6];

    while (1) {
        for (int i = 0; i < 20000; ++i)
            ;
        resetButtons();

        //MPU6050_GetRawAccelGyro(accelgyro);
        //sprintf(buffer, "%d\t%d\t%d | %d\t%d\t%d", accelgyro[0], accelgyro[1], accelgyro[2], accelgyro[3], accelgyro[4], accelgyro[5]);
        //printPlain(buffer);
        //HMC5883L_GetHeading(MagneticHeading);
        //sprintf(buffer, "%d\t%d\t%d\n", MagneticHeading[0], MagneticHeading[1], MagneticHeading[2]);
        //if (abs(accelgyro[1]) > 30000) {
        //flashlightToggle();
        //}
        //printPlain(buffer);
        //flashlightToggle();

        count++;
    }
}

