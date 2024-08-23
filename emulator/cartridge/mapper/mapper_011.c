#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


#define BANK_SELECT_PRG_ROM_BITS 0b00000011
#define BANK_SELECT_UNUSED_BITTS 0b00001100
#define BANK_SELECT_CHR_ROM_BITS 0b11110000


uint8_t Mapper011ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper011ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper011WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper011WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper011ScanlineIRQ(struct Cartridge* cartridge);


void Mapper011Init(struct Cartridge* cartridge) {
    struct Mapper011Info* mapper_info = (struct Mapper011Info*)cartridge->mapper_info;

    mapper_info->prg_rom_bank_offset = 0;
    mapper_info->chr_rom_bank_offset = 0;

    cartridge->MapperReadCPU = &Mapper011ReadCPU;
    cartridge->MapperReadPPU = &Mapper011ReadPPU;
    cartridge->MapperWriteCPU = &Mapper011WriteCPU;
    cartridge->MapperWritePPU = &Mapper011WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper011ScanlineIRQ;
}

uint8_t Mapper011ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper011Info* mapper_info = (struct Mapper011Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(ERROR, MAPPER, "Attempted read from unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted read from prg ram that's not supported by mapper 011\n");
    } else {
        return cartridge->prg_rom[(address & 0x7FFF) + mapper_info->prg_rom_bank_offset];
    }
}

uint8_t Mapper011ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper011Info* mapper_info = (struct Mapper011Info*)cartridge->mapper_info;
    return cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_bank_offset];
}

void Mapper011WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper011Info* mapper_info = (struct Mapper011Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted write to prg ram that's not supported by mapper 011\n");
    } else {
        mapper_info->prg_rom_bank_offset = (data & BANK_SELECT_PRG_ROM_BITS) * 0x8000;
        mapper_info->chr_rom_bank_offset = ((data & BANK_SELECT_CHR_ROM_BITS) >> 4) * 0x2000;
    }
}

void Mapper011WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper011Info* mapper_info = (struct Mapper011Info*)cartridge->mapper_info;
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_bank_offset] = data;
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper011ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}