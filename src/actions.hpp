#pragma once

enum ACTION {
    ACTION_NONE, ACTION_IGNORE
};

void processAction(int action);
void changeState(uint8_t option);
