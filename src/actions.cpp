#include "input_codes.hpp"
#include "indicator.hpp"

void processAction(int action) {
    switch (action) {
    case INPUT_J:
        flashlightToggle();
        break;
    case INPUT_I:
        presenceToggle();
        break;
    default:
        break;
    }
}
