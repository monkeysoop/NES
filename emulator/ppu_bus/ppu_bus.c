#include "ppu_bus.h"
#include <stdio.h>

#include "logger.h"

void PPUBusInit(struct PPUBus* ppu_bus, struct Cartridge* cartridge) {
    ppu_bus->cartridge = cartridge;
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
    memset(ppu_bus->ppu_vram, 0x00, VRAM_SIZE * sizeof(uint8_t));
}

void PPUBusReset(struct PPUBus* ppu_bus) {
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
    memset(ppu_bus->ppu_vram, 0x00, VRAM_SIZE * sizeof(uint8_t));
}

void PPUBusScanlineIRQ(struct PPUBus* ppu_bus) {
    CartridgeScanlineIRQ(ppu_bus->cartridge);
}

uint8_t PPUBusRead(struct PPUBus* ppu_bus, const uint16_t address) {
    if (address < 0x2000) {
        ppu_bus->ppu_vram_open_bus_data = CartridgeReadPPU(ppu_bus->cartridge, address);
        return ppu_bus->ppu_vram_open_bus_data;
    
    } else if (address < 0x3F00) {
        uint16_t vram_index = ppu_bus->cartridge->mirroring_offsets[((address & 0b0000110000000000) >> 10)] + (address & 0b0000001111111111);
        ppu_bus->ppu_vram_open_bus_data = ppu_bus->ppu_vram[vram_index];
        return ppu_bus->ppu_vram_open_bus_data;

    } else if (address < 0x4000) {
        return ppu_bus->palette[(address & 0x1F)];
    
    } else {
        LOG(ERROR, PPU_BUS, "Invalid address read 0x%04X\n", address);
    }
}

void PPUBusWrite(struct PPUBus* ppu_bus, const uint16_t address, const uint8_t data) {
    if (address < 0x2000) {
        CartridgeWritePPU(ppu_bus->cartridge, address, data);
        ppu_bus->ppu_vram_open_bus_data = data;
    
    } else if (address < 0x3F00) {
        uint16_t vram_index = ppu_bus->cartridge->mirroring_offsets[((address & 0b0000110000000000) >> 10)] + (address & 0b0000001111111111);
        ppu_bus->ppu_vram[vram_index] = data;
        ppu_bus->ppu_vram_open_bus_data = data;
    
    } else if (address < 0x4000) {
        ppu_bus->palette[(address & 0x1F)] = data;
        if ((address & 0x0003) == 0) {
            ppu_bus->palette[((address & 0x1F) ^ 0x10)] = data;   // mirroring
        }    

    } else {
        LOG(ERROR, PPU_BUS, "Invalid address written 0x%04X\n", address);
    }
}
