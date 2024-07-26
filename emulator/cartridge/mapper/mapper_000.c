#include <stdio.h>

#include "cartridge.h"


uint8_t Mapper000ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

void Mapper000Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units) {
    if (prg_rom_16KB_units == 1) {
        cartridge->cpu_address_mask = 0x3FFF;
    } else if (prg_rom_16KB_units == 2) {
        cartridge->cpu_address_mask = 0x7FFF;
    } else {
        printf("mapper 000 does not support more than 2 prg rom banks");
        exit(1);
    }

    cartridge->MapperReadCPU = &Mapper000ReadCPU;
    cartridge->MapperReadPPU = &Mapper000ReadPPU;
    cartridge->MapperWriteCPU = &Mapper000WriteCPU;
    cartridge->MapperWritePPU = &Mapper000WritePPU;

    cartridge->MapperScanlineIRQ = NULL;
}

uint8_t Mapper000ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    return (cartridge->prg_rom[(address & 0x7FFF) & cartridge->cpu_address_mask]);
}

uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    return cartridge->chr_rom[address];
}


void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    printf("Attempted write to prg rom on mapper 000\n");
    exit(1);
}

void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    printf("Attempted write to chr rom on mapper 000 (some games can have chr rams but it's not supported)\n");
    exit(1);
}
