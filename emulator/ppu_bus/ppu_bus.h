#ifndef PPU_BUS_H
#define PPU_BUS_H

#include <stdint.h>
#include <string.h>
#include "cartridge.h"



#define PALETTE_RAM_SIZE 0x20   // 32 bytes


struct PPUBus {
    uint8_t palette[PALETTE_RAM_SIZE];
    
    struct Cartridge* cartridge;
};

void PPUBusInit(struct PPUBus* ppu_bus, struct Cartridge* cartridge);
void PPUBusReset(struct PPUBus* ppu_bus);

void PPUBusScanlineIRQ(struct PPUBus* ppu_bus);

uint8_t PPUBusRead(struct PPUBus* ppu_bus, const uint16_t address);
void PPUBusWrite(struct PPUBus* ppu_bus, const uint16_t address, const uint8_t data);

#endif