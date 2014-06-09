#include "device.hpp"
#include "ring.hpp"
#include "stdio.h"
#include <stdlib.h>

int main(void) {
    configureDevice();
    int count = 0;

    while (1) {
        for (int i = 0; i < 20000; ++i)
            ;
        resetButtons();
        count++;
    }
}

