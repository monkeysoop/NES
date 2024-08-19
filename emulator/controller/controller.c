#include "controller.h"
#include "logger.h"


void ControllerInit(struct Controller* controller) {
    controller->strobe = 0;

    controller->status_1 = 0;
    controller->status_2 = 0;
    
    controller->real_status_1 = 0;
    controller->real_status_2 = 0;
}

void ControllerReset(struct Controller* controller) {
    controller->strobe = 0;
    
    controller->status_1 = 0;
    controller->status_2 = 0;
}



void ControllerKeyDown1(struct Controller* controller, enum Button button) {
    controller->real_status_1 |= button;
}

void ControllerKeyDown2(struct Controller* controller, enum Button button) {
    controller->real_status_2 |= button;
}

void ControllerKeyUp1(struct Controller* controller, enum Button button) {
    controller->real_status_1 &= ~button;
}

void ControllerKeyUp2(struct Controller* controller, enum Button button) {
    controller->real_status_2 &= ~button;
}



uint8_t ControllerRead1(struct Controller* controller) {
    uint8_t res = 0x00;
    switch (controller->strobe) {
        case 0: 
            res = controller->status_1 & 0x01;
            controller->status_1 >>= 1;
            controller->status_1 |= 0x80;
            break;
        case 1: 
            res |= (controller->real_status_1 & A);
            break; 
    }
    return res;
}

uint8_t ControllerRead2(struct Controller* controller) {
    uint8_t res = 0x00;
    switch (controller->strobe) {
        case 0: 
            res = controller->status_2 & 0x01;
            controller->status_2 >>= 1;
            controller->status_2 |= 0x80;
            break;
        case 1: 
            res = (controller->real_status_2 & A);
            break; 
    }
    return res;
}

void ControllerWrite(struct Controller* controller, uint8_t data) {
    controller->strobe = data & 0x01;

    if (controller->strobe == 0x01) {
        controller->status_1 = controller->real_status_1; 
        controller->status_2 = controller->real_status_2; 
    }
}