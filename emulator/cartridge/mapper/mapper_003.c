#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


uint8_t Mapper003ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper003ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper003WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper003WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper003ScanlineIRQ(struct Cartridge* cartridge);


void Mapper003Init(struct Cartridge* cartridge) {
    struct Mapper003Info* mapper_info = (struct Mapper003Info*)cartridge->mapper_info;
    if (cartridge->prg_rom_16KB_units == 1) {
        mapper_info->prg_rom_mask = 0x3FFF;
    } else if (cartridge->prg_rom_16KB_units == 2) {
        mapper_info->prg_rom_mask = 0x7FFF;
    } else {
        LOG(ERROR, MAPPER, "mapper 003 does not support more than 2 16KB prg rom banks");
    }
    mapper_info->chr_rom_offset = 0x00000000;

    cartridge->MapperReadCPU = &Mapper003ReadCPU;
    cartridge->MapperReadPPU = &Mapper003ReadPPU;
    cartridge->MapperWriteCPU = &Mapper003WriteCPU;
    cartridge->MapperWritePPU = &Mapper003WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper003ScanlineIRQ;
}

uint8_t Mapper003ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper003Info* mapper_info = (struct Mapper003Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(ERROR, MAPPER, "Attempted read from unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted read from prg ram that's not supported by mapper 003\n");
    } else {
        return cartridge->prg_rom[address & mapper_info->prg_rom_mask];
    }
}

uint8_t Mapper003ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper003Info* mapper_info = (struct Mapper003Info*)cartridge->mapper_info;
    return cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_offset];
}

void Mapper003WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper003Info* mapper_info = (struct Mapper003Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(ERROR, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted write to prg ram that's not supported by mapper 003\n");
    } else {
        mapper_info->chr_rom_offset = 0x2000 * data;
    }
}

void Mapper003WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper003Info* mapper_info = (struct Mapper003Info*)cartridge->mapper_info;
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_offset] = data;
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper003ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}