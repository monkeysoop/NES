#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "cpu_bus.h"

#define STACK_OFFSET 0x0100
#define RESET_OFFSET 0xFFFC
#define BREAK_INTERRUPT_OFFSET 0xFFFE
#define NON_MASKABLE_INTERRUPT_OFFSET 0xFFFA


typedef enum {
    CARRY =         0b00000001,
    ZERO =          0b00000010,
    IRQ_DISABLE =   0b00000100,
    DECIMAL_MODE =  0b00001000,
    BRK_COMMAND =   0b00010000,
    UNUSED =        0b00100000,
    OVERFLOW =      0b01000000,
    NEGATIVE =      0b10000000
} StatusFlags;


typedef struct Registers {
    uint8_t a_register;
	uint8_t x_register; 
	uint8_t y_register;   
    uint8_t status_flags;
    uint8_t stack_pointer; 
	uint16_t program_counter;
} Registers;

typedef struct CPU {
    uint8_t remaining_cycles;
    uint64_t tick_counter;

    Registers registers;

    CPUBus* cpu_bus;
} CPU;


void Init(CPU* cpu);
void Reset(CPU* cpu);

void InterruptRequest(CPU* cpu);
void NonMaskableInterrupt(CPU* cpu);

void tick(CPU* cpu);



#endif