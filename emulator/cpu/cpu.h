#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "cpu_bus.h"


#define ZERO_PAGE_BYTE_WIDTH 3

#define ZERO_PAGE_BYTE_BUFFER_WIDTH 16
#define ZERO_PAGE_BYTE_BUFFER_HEIGHT 16

#define REGISTER_WIDTH 23
#define REGISTERS_BUFFER_HEIGHT 13 

#define DISASSEMBLY_BUFFER_WIDTH 32
#define DISASSEMBLY_BUFFER_HEIGHT 48


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

struct Registers {
    uint8_t a_register;
	uint8_t x_register; 
	uint8_t y_register;   
    uint8_t status_flags;
    uint8_t stack_pointer; 
	uint16_t program_counter;
};

struct CPU {
    uint8_t remaining_cycles;
    uint64_t tick_counter;

    bool dma_transfer;
    bool dma_aligned;
    uint16_t dma_address;
    uint8_t oam_address;
    uint8_t oam_data;

    struct Registers registers;

    struct CPUBus* cpu_bus;
};


void CPUInit(struct CPU* cpu, struct CPUBus* cpu_bus);
void CPUReset(struct CPU* cpu);

void CPUInterruptRequest(struct CPU* cpu);
void CPUNonMaskableInterrupt(struct CPU* cpu);

void CPUClock(struct CPU* cpu);

uint8_t CPUDisassemble(
    struct CPU* cpu, uint16_t start_address, uint16_t count, 
    char disassembly_buffer[DISASSEMBLY_BUFFER_HEIGHT][DISASSEMBLY_BUFFER_WIDTH],
    char zero_page_buffer[ZERO_PAGE_BYTE_BUFFER_HEIGHT][ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH],
    char registers_buffer[REGISTERS_BUFFER_HEIGHT][REGISTER_WIDTH]
);

#endif