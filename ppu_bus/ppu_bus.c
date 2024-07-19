#include "ppu_bus.h"

void PPUBusInit(PPUBus* ppu_bus, Cartridge* cartridge) {
    ppu_bus->cartridge = cartridge;
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
}

void PPUBusReset(PPUBus* ppu_bus) {
    memset(ppu_bus->palette, 0x00, PALETTE_RAM_SIZE * sizeof(uint8_t));
}

void PPUBusScanlineIRQ(PPUBus* ppu_bus) {
    CartridgeScanlineIRQ(ppu_bus->cartridge);
}

uint8_t PPUBusRead(PPUBus* ppu_bus, const uint16_t address) {
    if (address < 0x3000) {
        return CartridgeReadPPU(ppu_bus->cartridge, address);
    } else if (address < 0x3F00) {
        return CartridgeReadPPU(ppu_bus->cartridge, (address - 0x1000));
    } else if (address < 0x3FFF) {
        return ppu_bus->palette[(address & 0x1F)];
    } else {
        printf("Invalid address\n");
        exit(1);
    }
    
}

void PPUBusWrite(PPUBus* ppu_bus, const uint16_t address, const uint8_t data) {

}
