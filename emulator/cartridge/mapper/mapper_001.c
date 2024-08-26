#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


#define SHIFT_REGISTER_ACTIVE_BITS 0b00011111
#define SHIFT_REGISTER_UNUSED_BITS 0b11100000

#define LOAD_REGISTER_DATA_BIT    0b00000001
#define LOAD_REGISTER_UNUSED_BITS 0b01111110
#define LOAD_REGISTER_RESET_BIT   0b10000000

#define CONTROL_REGISTER_MIRRORING_BITS         0b00000011
#define CONTROL_REGISTER_PRG_ROM_BANK_MODE_BITS 0b00001100
#define CONTROL_REGISTER_CHR_ROM_BANK_MODE_BIT  0b00010000
#define CONTROL_REGISTER_UNUSED_BITS            0b11100000

#define PRG_REGISTER_BANK_SELECT_BITS 0b00001111


uint8_t Mapper001ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper001ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper001WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper001WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper001ScanlineIRQ(struct Cartridge* cartridge);

static void SetCHRBanks(struct Cartridge* cartridge);
static void SetPRGBanks(struct Cartridge* cartridge);


void Mapper001Init(struct Cartridge* cartridge) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;

    mapper_info->shift_register = 0;
    mapper_info->control_register = 0b00001100; // mode 3 for prg

    mapper_info->chr_1_register = 0;
    mapper_info->chr_2_register = 0;
    mapper_info->prg_register = 0;

    mapper_info->shift_register_counter = 0;
    
    
    mapper_info->prg_rom_bank_1_offset = 0;
    mapper_info->prg_rom_bank_2_offset = (cartridge->prg_rom_16KB_units - 1) * 0x4000;

    mapper_info->chr_rom_bank_1_offset = 0;
    mapper_info->chr_rom_bank_2_offset = 0x1000;


    cartridge->MapperReadCPU = &Mapper001ReadCPU;
    cartridge->MapperReadPPU = &Mapper001ReadPPU;
    cartridge->MapperWriteCPU = &Mapper001WriteCPU;
    cartridge->MapperWritePPU = &Mapper001WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper001ScanlineIRQ;
}

uint8_t Mapper001ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted read from unmapped area\n");
        return 0;
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted read from prg ram that hase size 0\n");
        } else {
            return cartridge->prg_ram[address & 0x1FFF];
        }
    } else if (address < 0xC000) {
        return cartridge->prg_rom[(address & 0x3FFF) + mapper_info->prg_rom_bank_1_offset];
    } else {
        return cartridge->prg_rom[(address & 0x3FFF) + mapper_info->prg_rom_bank_2_offset];
    }
}

uint8_t Mapper001ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    if (address < 0x1000) {
        return cartridge->chr_rom[(address & 0x0FFF) + mapper_info->chr_rom_bank_1_offset];
    } else {
        return cartridge->chr_rom[(address & 0x0FFF) + mapper_info->chr_rom_bank_2_offset];
    }
}

void Mapper001WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted write to prg ram that has size 0\n");
        } else {
            cartridge->prg_ram[address & 0x1FFF] = data;
        }
    } else {
        if (data & LOAD_REGISTER_RESET_BIT) {
            mapper_info->shift_register = 0;
            mapper_info->shift_register_counter = 0;
            mapper_info->control_register |= 0b00001100;
            SetPRGBanks(cartridge);
            return;
        }
        mapper_info->shift_register >>= 1;
        mapper_info->shift_register |= (data & LOAD_REGISTER_DATA_BIT) << 4;
        mapper_info->shift_register_counter++;

        if (mapper_info->shift_register_counter == 5) {
            switch (address & 0b1110000000000000) {
                case 0x8000: 
                    mapper_info->control_register = mapper_info->shift_register & SHIFT_REGISTER_ACTIVE_BITS;

                    switch (mapper_info->control_register & CONTROL_REGISTER_MIRRORING_BITS) {
                        case 0: CartridgeSetMirroring(cartridge, ONE_SCREEN_LOWER_MIRRORING); break;
                        case 1: CartridgeSetMirroring(cartridge, ONE_SCREEN_UPPER_MIRRORING); break;
                        case 2: CartridgeSetMirroring(cartridge, VERTICAL_MIRRORING); break;
                        case 3: CartridgeSetMirroring(cartridge, HORIZONTAL_MIRRORING); break;
                    }

                    SetCHRBanks(cartridge);
                    SetPRGBanks(cartridge);
                    break;
                case 0xA000: 
                    mapper_info->chr_1_register = mapper_info->shift_register & SHIFT_REGISTER_ACTIVE_BITS;       
                    SetCHRBanks(cartridge);
                    break;
                case 0xC000: 
                    mapper_info->chr_2_register = mapper_info->shift_register & SHIFT_REGISTER_ACTIVE_BITS;       
                    SetCHRBanks(cartridge);
                    break;
                case 0xE000: 
                    mapper_info->prg_register = mapper_info->shift_register & SHIFT_REGISTER_ACTIVE_BITS & PRG_REGISTER_BANK_SELECT_BITS;       
                    SetPRGBanks(cartridge);
                    break;
            }
            
            mapper_info->shift_register = 0;
            mapper_info->shift_register_counter = 0;
        }
    }
}

void Mapper001WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    if (cartridge->supports_chr_ram) {
        if (address < 0x1000) {
            cartridge->chr_rom[(address & 0x0FFF) + mapper_info->chr_rom_bank_1_offset] = data;
        } else {
            cartridge->chr_rom[(address & 0x0FFF) + mapper_info->chr_rom_bank_2_offset] = data;
        }
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper001ScanlineIRQ(struct Cartridge* cartridge) {
    return false;
}


static void SetCHRBanks(struct Cartridge* cartridge) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    switch ((mapper_info->control_register & CONTROL_REGISTER_CHR_ROM_BANK_MODE_BIT) >> 4) {
        case 0:
            // one 8KB mode
            mapper_info->chr_rom_bank_1_offset = (mapper_info->chr_1_register & 0b11111110) * 0x1000;
            mapper_info->chr_rom_bank_2_offset = ((mapper_info->chr_1_register & 0b11111110) + 1) * 0x1000;
            break;
        case 1:
            // two 4KB mode
            mapper_info->chr_rom_bank_1_offset = mapper_info->chr_1_register * 0x1000;
            mapper_info->chr_rom_bank_2_offset = mapper_info->chr_2_register * 0x1000;
            break;
    }
}

static void SetPRGBanks(struct Cartridge* cartridge) {
    struct Mapper001Info* mapper_info = (struct Mapper001Info*)cartridge->mapper_info;
    switch ((mapper_info->control_register & CONTROL_REGISTER_PRG_ROM_BANK_MODE_BITS) >> 2) {
        case 0:
        case 1: 
            // one 32KB mode
            mapper_info->prg_rom_bank_1_offset = (mapper_info->prg_register & 0b11111110) * 0x4000;
            mapper_info->prg_rom_bank_2_offset = ((mapper_info->prg_register & 0b11111110) + 1) * 0x4000;
            break;
        case 2: 
            // two 16KB mode
            mapper_info->prg_rom_bank_1_offset = 0;
            mapper_info->prg_rom_bank_2_offset = mapper_info->prg_register * 0x4000;
            break;
        case 3: 
            // two 16KB mode
            mapper_info->prg_rom_bank_1_offset = mapper_info->prg_register * 0x4000;
            mapper_info->prg_rom_bank_2_offset = (cartridge->prg_rom_16KB_units - 1) * 0x4000;
            break;
    }
}