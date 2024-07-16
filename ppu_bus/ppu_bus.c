#include "ppu_bus.h"

void InitPPUBus(PPUBus* ppu_bus, Cartridge* cartridge) {
    ppu_bus->cartridge = cartridge;
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
}

void ResetPPUBus(PPUBus* ppu_bus) {
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
}


uint8_t ReadPPUBus(PPUBus* ppu_bus, const uint16_t address) {
    if (address < 0x3000) {
        return PPUReadCartridge(ppu_bus->cartridge, address);
    } else if (address < 0x3F00) {
        return PPUReadCartridge(ppu_bus->cartridge, (address - 0x1000));
    } else if (address < 0x3FFF) {
        return ppu_bus->palette[(address & 0x1F)];
    } else {
        printf("Invalid address\n");
        exit(1);
    }
    
}

void WritePPUBus(PPUBus* ppu_bus, const uint16_t address, const uint8_t data) {

}
