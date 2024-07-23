#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "cpu_bus.h"

#define STACK_POINTER_OFFSET 0xFD
#define STACK_OFFSET 0x0100
#define RESET_OFFSET 0xFFFC
#define BREAK_INTERRUPT_OFFSET 0xFFFE
#define NON_MASKABLE_INTERRUPT_OFFSET 0xFFFA


#define CARRY 0b00000001
#define ZERO 0b00000010
#define IRQ_DISABLE 0b00000100
#define DECIMAL_MODE 0b00001000
#define BRK_COMMAND 0b00010000
#define UNUSED 0b00100000
#define OVERFLOW 0b01000000
#define NEGATIVE 0b10000000

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


void CPUInit(CPU* cpu);
void CPUReset(CPU* cpu);

void CPUInterruptRequest(CPU* cpu);
void CPUNonMaskableInterrupt(CPU* cpu);

void CPUClock(CPU* cpu);

uint8_t CPUDisassemble(CPU* cpu, uint16_t start_address, uint16_t count, char** debug_char_buffer, uint8_t w, uint8_t h);

#endif