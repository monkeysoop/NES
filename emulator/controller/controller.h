#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

enum Button {
    A =      0b00000001,
    B =      0b00000010,
    SELECT = 0b00000100,
    START =  0b00001000,
    UP =     0b00010000,
    DOWN =   0b00100000,
    LEFT =   0b01000000,
    RIGHT =  0b10000000,
};

struct Controller {
    uint8_t strobe;

    uint8_t status_1;
    uint8_t status_2;
    
    uint8_t real_status_1;
    uint8_t real_status_2;
};

void ControllerInit(struct Controller* controller);
void ControllerReset(struct Controller* controller);

void ControllerKeyDown1(struct Controller* controller, enum Button button);
void ControllerKeyDown2(struct Controller* controller, enum Button button);
void ControllerKeyUp1(struct Controller* controller, enum Button button);
void ControllerKeyUp2(struct Controller* controller, enum Button button);


uint8_t ControllerRead1(struct Controller* controller);
uint8_t ControllerRead2(struct Controller* controller);
void ControllerWrite(struct Controller* controller, uint8_t data);
#endif