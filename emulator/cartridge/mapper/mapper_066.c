#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


#define BANK_SELECT_CHR_ROM_BITS 0b00000011
#define BANK_SELECT_PRG_ROM_BITS 0b00110000
#define BANK_SELECT_UNUSED_BITTS 0b11001100


uint8_t Mapper066ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper066ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper066WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper066WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper066ScanlineIRQ(struct Cartridge* cartridge);


void Mapper066Init(struct Cartridge* cartridge) {
    struct Mapper066Info* mapper_info = (struct Mapper066Info*)cartridge->mapper_info;

    mapper_info->prg_rom_bank_offset = 0;
    mapper_info->chr_rom_bank_offset = 0;

    cartridge->MapperReadCPU = &Mapper066ReadCPU;
    cartridge->MapperReadPPU = &Mapper066ReadPPU;
    cartridge->MapperWriteCPU = &Mapper066WriteCPU;
    cartridge->MapperWritePPU = &Mapper066WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper066ScanlineIRQ;
}

uint8_t Mapper066ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper066Info* mapper_info = (struct Mapper066Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(ERROR, MAPPER, "Attempted read from unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted read from prg ram that's not supported by mapper 066\n");
    } else {
        return cartridge->prg_rom[(address & 0x7FFF) + mapper_info->prg_rom_bank_offset];
    }
}

uint8_t Mapper066ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper066Info* mapper_info = (struct Mapper066Info*)cartridge->mapper_info;
    return cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_bank_offset];
}

void Mapper066WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper066Info* mapper_info = (struct Mapper066Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(ERROR, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        LOG(ERROR, MAPPER, "Attempted write to prg ram that's not supported by mapper 066\n");
    } else {
        mapper_info->prg_rom_bank_offset = ((data & BANK_SELECT_PRG_ROM_BITS) >> 4) * 0x8000;
        mapper_info->chr_rom_bank_offset = (data & BANK_SELECT_CHR_ROM_BITS) * 0x2000;
    }
}

void Mapper066WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper066Info* mapper_info = (struct Mapper066Info*)cartridge->mapper_info;
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[(address & 0x1FFF) + mapper_info->chr_rom_bank_offset] = data;
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper066ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}