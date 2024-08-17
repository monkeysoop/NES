#include <stdio.h>

#include "cartridge.h"


uint8_t Mapper000ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper000ScanlineIRQ(struct Cartridge* cartridge);

void Mapper000Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units) {
    struct Mapper000Info* mapper_info = (struct Mapper000Info*)cartridge->mapper_info;
    if (prg_rom_16KB_units == 1) {
        mapper_info->prg_rom_mask = 0x3FFF;
    } else if (prg_rom_16KB_units == 2) {
        mapper_info->prg_rom_mask = 0x7FFF;
    } else {
        printf("mapper 000 does not support more than 2 prg rom banks");
        exit(1);
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
        printf("Attempted read from unmapped area\n");
        exit(1);
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            printf("Attempted read from prg ram that hase size 0\n");
            exit(1);
        } else {
            return cartridge->prg_ram[address & 0x0FFF];
        }
    } else {
        return cartridge->prg_rom[address & mapper_info->prg_rom_mask];
    }
}

uint8_t Mapper000ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    return cartridge->chr_rom[address];
}


void Mapper000WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (address < 0x6000) {
        printf("Attempted write to unmapped area\n");
        exit(1);
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            printf("Attempted write to prg ram that hase size 0\n");
            exit(1);
        } else {
            cartridge->prg_ram[address & 0x0FFF] = data;
        }
    } else {
        printf("Attempted write to prg rom\n");
        exit(1);
    }
}

void Mapper000WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[address] = data;
    } else {
        printf("Attempted write to chr rom\n");
        exit(1);
    }
}


void Mapper000ScanlineIRQ(struct Cartridge* cartridge) {
}