#include "input_codes.hpp"
#include "indicator.hpp"
#include "actions.hpp"

int currentAction = ACTION_NONE;
bool ignoreTable[36] = { }; // XXX bools are inefficient

void processAction(int action) {
    // TODO check action range

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
