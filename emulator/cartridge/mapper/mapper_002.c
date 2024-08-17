#include <stdio.h>

#include "cartridge.h"


uint8_t Mapper002ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper002ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper002WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper002WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper002ScanlineIRQ(struct Cartridge* cartridge);

void Mapper002Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units) {
    struct Mapper002Info* mapper_info = (struct Mapper002Info*)cartridge->mapper_info;
    mapper_info->prg_rom_bank_1_offset = 0x0000;
    mapper_info->prg_rom_bank_2_offset = (prg_rom_16KB_units - 1) * 0x4000;

    cartridge->MapperReadCPU = &Mapper002ReadCPU;
    cartridge->MapperReadPPU = &Mapper002ReadPPU;
    cartridge->MapperWriteCPU = &Mapper002WriteCPU;
    cartridge->MapperWritePPU = &Mapper002WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper002ScanlineIRQ;
}

uint8_t Mapper002ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper002Info* mapper_info = (struct Mapper002Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        printf("Attempted read from unmapped area\n");
        exit(1);
    } else if (address < 0x8000) {
        printf("Attempted read from prg ram that's not supported by mapper 002\n");
        exit(1);
    } else if (address < 0xC000) {
        return cartridge->prg_rom[(address & 0x3FFF) + mapper_info->prg_rom_bank_1_offset];
    } else {
        return cartridge->prg_rom[(address & 0x3FFF) + mapper_info->prg_rom_bank_2_offset];
    }
}

uint8_t Mapper002ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    return cartridge->chr_rom[address];
}


void Mapper002WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper002Info* mapper_info = (struct Mapper002Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        printf("Attempted write to unmapped area\n");
        exit(1);
    } else if (address < 0x8000) {
        printf("Attempted write to prg ram that's not supported by mapper 002\n");
        exit(1);
    } else {
        mapper_info->prg_rom_bank_1_offset = (data & 0b00000111) * 0x4000;
    }
}

void Mapper002WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[address] = data;
    } else {
        printf("Attempted write to chr rom\n");
        exit(1);
    }
}


void Mapper002ScanlineIRQ(struct Cartridge* cartridge) {
}