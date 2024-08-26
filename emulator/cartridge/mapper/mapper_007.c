#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


#define BANK_SELECT_PRG_ROM_BITS  0b00000111
#define BANK_SELECT_MIRRORING_BIT 0b00010000
#define BANK_SELECT_UNUSED_BITTS  0b11101000


uint8_t Mapper007ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper007ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper007WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper007WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper007ScanlineIRQ(struct Cartridge* cartridge);


void Mapper007Init(struct Cartridge* cartridge) {
    struct Mapper007Info* mapper_info = (struct Mapper007Info*)cartridge->mapper_info;

    mapper_info->prg_rom_bank_offset = 0;

    cartridge->MapperReadCPU = &Mapper007ReadCPU;
    cartridge->MapperReadPPU = &Mapper007ReadPPU;
    cartridge->MapperWriteCPU = &Mapper007WriteCPU;
    cartridge->MapperWritePPU = &Mapper007WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper007ScanlineIRQ;
}

uint8_t Mapper007ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper007Info* mapper_info = (struct Mapper007Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted read from unmapped area\n");
        return 0;
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted read from prg ram that hase size 0\n");
        } else {
            return cartridge->prg_ram[address & 0x1FFF];
        }
    } else {
        return cartridge->prg_rom[(address & 0x7FFF) + mapper_info->prg_rom_bank_offset];
    }
}

uint8_t Mapper007ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    return cartridge->chr_rom[address & 0x1FFF];
}

void Mapper007WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper007Info* mapper_info = (struct Mapper007Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted write to prg ram that has size 0\n");
        } else {
            cartridge->prg_ram[address & 0x1FFF] = data;
        }
    } else {
        mapper_info->prg_rom_bank_offset = (data & BANK_SELECT_PRG_ROM_BITS) * 0x8000;
        if (data & BANK_SELECT_MIRRORING_BIT) {
            CartridgeSetMirroring(cartridge, ONE_SCREEN_UPPER_MIRRORING);
        } else {
            CartridgeSetMirroring(cartridge, ONE_SCREEN_LOWER_MIRRORING);
        }
    }
}

void Mapper007WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    if (cartridge->supports_chr_ram) {
        cartridge->chr_rom[address & 0x1FFF] = data;
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper007ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}