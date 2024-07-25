#ifndef CPU_BUS_H
#define CPU_BUS_H

#include <stdint.h>

#include "cartridge.h"
#include "ppu.h"

#define CPU_RAM_SIZE 0x7FF  // 2KB


#define PPU_CTRL 0x2000
#define PPU_MASK 0x2001
#define PPU_STATUS 0x2002
#define OAM_ADDRESS 0x2003
#define OAM_DATA 0x2004
#define PPU_SCROLL 0x2005
#define PPU_ADDRESS 0x2006
#define PPU_DATA 0x2007

#define APU_PULSE_1_CTRL 0x4000
#define APU_PULSE_1_SWEEP 0x4001
#define APU_PULSE_1_LOW_BYTE 0x4002
#define APU_PULSE_1_HIGH_BYTE 0x4003

#define APU_PULSE_2_CTRL 0x4004
#define APU_PULSE_2_SWEEP 0x4005
#define APU_PULSE_2_LOW_BYTE 0x4006
#define APU_PULSE_2_HIGH_BYTE 0x4007

#define APU_TRIANGLE_CTRL 0x4008
#define APU_TRIANGLE_UNUSED 0x4009
#define APU_TRIANGLE_LOW_BYTE 0x400A
#define APU_TRIANGLE_HIGH_BYTE 0x400B

#define APU_NOISE_CTRL 0x400C
#define APU_NOISE_UNUSED 0x400D
#define APU_NOISE_LOW_BYTE 0x400E
#define APU_NOISE_HIGH_BYTE 0x400F

#define APU_DMC_CTRL 0x4010
#define APU_DMC_DIRECT_LOAD 0x4011
#define APU_DMC_ADDRESS 0x4012
#define APU_DMC_LENGTH 0x4013

#define PPU_DMA 0x4014

#define APU_CTRL 0x4015
#define APU_STATUS 0x4015

#define JOYSTICK_STROBE 0x4016
#define JOYSTICK_1_DATA 0x4016

#define APU_FRAME_COUNTER 0x4017
#define JOYSTICK_2_DATA 0x4017




struct CPUBus {
    uint8_t cpu_ram[CPU_RAM_SIZE];

    uint8_t cpu_open_bus_data;
    uint8_t ppu_io_open_bus_data;

    struct Cartridge* cartridge;
    struct PPU* ppu;
    
};

void CPUBusInit(struct CPUBus* cpu_bus, struct Cartridge* cartridge, struct PPU* ppu);
void CPUBusReset(struct CPUBus* cpu_bus);


uint8_t CPUBusRead(struct CPUBus* cpu_bus, const uint16_t address);
void CPUBusWrite(struct CPUBus* cpu_bus, const uint16_t address, const uint8_t data);

#endif