#include "device.hpp"
#include "ring.hpp"
#include "connection.hpp"
#include "stdio.h"
#include <stdlib.h>
#include "compass.hpp"

int main(void) {
    configureDevice();
    int count = 0;

    while (1) {
        resetButtons();
        processIncomingData();
        updateCompass();
        count++;
    }
}

