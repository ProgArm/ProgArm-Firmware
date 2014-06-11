#include "input_codes.hpp"
#include "indicator.hpp"
#include "actions.hpp"

int currentAction = ACTION_NONE;
bool deviceActionsEnabled = true;
bool ignoreTable[36] = { }; // XXX bools are stored inefficiently

void changeState(uint8_t option) {
    switch (option) {
    case 0:
    case '0':
        deviceActionsEnabled = false;
        break;
    case 1:
    case '1':
        deviceActionsEnabled = true;
        break;
    case 2:
    case '2':
        deviceActionsEnabled = !deviceActionsEnabled;
        break;
    }
}

void processAction(int action) {
    // TODO check action range
    if (!deviceActionsEnabled)
        return;

    if (currentAction != ACTION_NONE) {
        switch (currentAction) {
        case ACTION_IGNORE:
            ignoreTable[action] = !ignoreTable[action];
            currentAction = ACTION_NONE;
            break;
        default:
            break;
        }
    } else {
        if (ignoreTable[action]) // this action should be ignored
            return;
        switch (action) {
        case INPUT_K:
            currentAction = ACTION_IGNORE;
            break;
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
}
