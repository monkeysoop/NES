#ifndef PPU_BUS_H
#define PPU_BUS_H

#include <stdint.h>
#include <string.h>
#include "cartridge.h"



#define PALETTE_RAM_SIZE 0x20   // 32 bytes


typedef struct PPUBus {
    uint8_t palette[PALETTE_RAM_SIZE];
    
    Cartridge* cartridge;
} PPUBus;

void PPUBusInit(PPUBus* ppu_bus, Cartridge* cartridge);
void PPUBusReset(PPUBus* ppu_bus);

void PPUBusScanlineIRQ(PPUBus* ppu_bus);

uint8_t PPUBusRead(PPUBus* ppu_bus, const uint16_t address);
void PPUBusWrite(PPUBus* ppu_bus, const uint16_t address, const uint8_t data);

#endif