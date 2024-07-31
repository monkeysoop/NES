#ifndef EMULATOR_H
#define EMULATOR_H

#include "cartridge.h"
#include "cpu.h"
#include "cpu_bus.h"
#include "ppu.h"
#include "ppu_bus.h"

struct Emulator {
    struct Cartridge cartridge; 
    struct CPU cpu; 
    struct CPUBus cpu_bus; 
    struct PPU ppu; 
    struct PPUBus ppu_bus;
};


void EmulatorInit(struct Emulator* emulator, const char* filename);
void EmulatorClean(struct Emulator* emulator);

void EmulatorReset(struct Emulator* emulator);

void EmulatorReloadCartridge(struct Emulator* emulator, const char* filename);

void EmulatorRender(struct Emulator* emulator, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]);

#endif