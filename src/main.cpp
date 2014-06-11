#include "device.hpp"
#include "ring.hpp"
#include "connection.hpp"
#include "stdio.h"
#include <stdlib.h>

int main(void) {
    configureDevice();
    int count = 0;

    while (1) {
        resetButtons();
        processIncomingData();
        count++;
    }
}

