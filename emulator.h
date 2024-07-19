#ifndef EMULATOR_H
#define EMULATOR_H

#include "cartridge.h"
#include "cpu.h"
#include "cpu_bus.h"
#include "ppu.h"
#include "ppu_bus.h"

typedef struct Emulator {
    Cartridge cartridge; 
    CPU cpu; 
    CPUBus cpu_bus; 
    PPU ppu; 
    PPUBus ppu_bus;
} Emulator;


void EmulatorInit(Emulator* emulator, const char* filename);
void EmulatorClean(Emulator* emulator);

void EmulatorReset(Emulator* emulator);

void EmulatorReloadCartridge(Emulator* emulator, const char* filename);

void EmulatorTick(Emulator* emulator);

#endif