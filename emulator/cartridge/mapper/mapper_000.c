#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


uint8_t Mapper000ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper000ScanlineIRQ(struct Cartridge* cartridge);


void Mapper000Init(struct Cartridge* cartridge) {
    struct Mapper000Info* mapper_info = (struct Mapper000Info*)cartridge->mapper_info;
    if (cartridge->prg_rom_16KB_units == 1) {
        mapper_info->prg_rom_mask = 0x3FFF;
    } else if (cartridge->prg_rom_16KB_units == 2) {
        mapper_info->prg_rom_mask = 0x7FFF;
    } else {
        LOG(ERROR, MAPPER, "mapper 000 does not support more than 2 16KB prg rom banks");
    }

    cartridge->MapperReadCPU = &Mapper000ReadCPU;
    cartridge->MapperReadPPU = &Mapper000ReadPPU;
    cartridge->MapperWriteCPU = &Mapper000WriteCPU;
    cartridge->MapperWritePPU = &Mapper000WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper000ScanlineIRQ;
}

uint8_t Mapper000ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper000Info* mapper_info = (struct Mapper000Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted read from unmapped area\n");
        return 0;
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted read from prg ram that hase size 0\n");
        } else {
            return cartridge->prg_ram[address & 0x0FFF];    // mapper officially only supports 2 or 4 KB of memmory
        }
    } else {
        return cartridge->prg_rom[address & mapper_info->prg_rom_mask];
    }
}

uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    return cartridge->chr_rom[address & 0x1FFF];
}

void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted write to prg ram that has size 0\n");
        } else {
            cartridge->prg_ram[address & 0x0FFF] = data;    // mapper officially only supports 2 or 4 KB of memmory
        }
    } else {
        LOG(ERROR, MAPPER, "Attempted write to prg rom\n");
    }
}

void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[address & 0x1FFF] = data;
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper000ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}