#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


#define BANK_SELECT_REGISTER_BANK_BITS         0b00000111
#define BANK_SELECT_REGISTER_UNUSED_BITS       0b00111000
#define BANK_SELECT_REGISTER_PRG_ROM_MODE_BIT  0b01000000
#define BANK_SELECT_REGISTER_CHR_INVERSION_BIT 0b10000000

#define MIRRORING_BIT         0b00000001
#define MIRRORING_UNUSED_BITS 0b11111110


uint8_t Mapper004ReadCPU(struct Cartridge* cartridge, uint16_t address);
uint8_t Mapper004ReadPPU(struct Cartridge* cartridge, uint16_t address);
void Mapper004WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);
void Mapper004WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data);

bool Mapper004ScanlineIRQ(struct Cartridge* cartridge);

static void SetPRGCHRBanks(struct Cartridge* cartridge);
static void SwapPRGROMMode(struct Cartridge* cartridge);
static void SwapCHRInversion(struct Cartridge* cartridge);


void Mapper004Init(struct Cartridge* cartridge) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;

    mapper_info->bank_select_register = 0;
    mapper_info->bank_select_register_previous = 0;
    mapper_info->bank_value_register = 0;

    mapper_info->irq_latch_register = 0;
    mapper_info->irq_counter_register = 0;
    mapper_info->irq_enabled = false;
    mapper_info->irq_reload_latch = false;

    mapper_info->prg_rom_bank_1_offset = 0;
    mapper_info->prg_rom_bank_2_offset = 0;
    mapper_info->prg_rom_bank_3_offset = (cartridge->prg_rom_16KB_units - 1) * 0x4000;
    mapper_info->prg_rom_bank_4_offset = (cartridge->prg_rom_16KB_units - 1) * 0x4000 + 0x2000;

    mapper_info->chr_rom_bank_1_offset = 0;
    mapper_info->chr_rom_bank_2_offset = 0;
    mapper_info->chr_rom_bank_3_offset = 0;
    mapper_info->chr_rom_bank_4_offset = 0;
    mapper_info->chr_rom_bank_5_offset = 0;
    mapper_info->chr_rom_bank_6_offset = 0;
    mapper_info->chr_rom_bank_7_offset = 0;
    mapper_info->chr_rom_bank_8_offset = 0;

    cartridge->MapperReadCPU = &Mapper004ReadCPU;
    cartridge->MapperReadPPU = &Mapper004ReadPPU;
    cartridge->MapperWriteCPU = &Mapper004WriteCPU;
    cartridge->MapperWritePPU = &Mapper004WritePPU;

    cartridge->MapperScanlineIRQ = &Mapper004ScanlineIRQ;
}

uint8_t Mapper004ReadCPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted read from unmapped area\n");
        return 0;
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted read from prg ram that hase size 0\n");
        } else {
            return cartridge->prg_ram[address & 0x1FFF];
        }
    } else if (address < 0xA000) {
        return cartridge->prg_rom[(address & 0x1FFF) + mapper_info->prg_rom_bank_1_offset];
    } else if (address < 0xC000) {
        return cartridge->prg_rom[(address & 0x1FFF) + mapper_info->prg_rom_bank_2_offset];
    } else if (address < 0xE000) {
        return cartridge->prg_rom[(address & 0x1FFF) + mapper_info->prg_rom_bank_3_offset];
    } else {
        return cartridge->prg_rom[(address & 0x1FFF) + mapper_info->prg_rom_bank_4_offset];
    }
}

uint8_t Mapper004ReadPPU(struct Cartridge* cartridge, uint16_t address) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    if (address < 0x0400) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_1_offset];
    } else if (address < 0x0800) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_2_offset];
    } else if (address < 0x0C00) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_3_offset];
    } else if (address < 0x1000) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_4_offset];
    } else if (address < 0x1400) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_5_offset];
    } else if (address < 0x1800) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_6_offset];
    } else if (address < 0x1C00) {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_7_offset];
    } else {
        return cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_8_offset];
    }
}

void Mapper004WriteCPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    if (address < 0x6000) {
        LOG(WARNING, MAPPER, "Attempted write to unmapped area\n");
    } else if (address < 0x8000) {
        if (cartridge->prg_ram_8KB_units == 0) {
            LOG(ERROR, MAPPER, "Attempted write to prg ram that has size 0\n");
        } else {
            cartridge->prg_ram[address & 0x1FFF] = data;
        }
    } else {
        switch (address & 0b1110000000000001) {
            case 0x8000:
                mapper_info->bank_select_register_previous = mapper_info->bank_select_register;
                mapper_info->bank_select_register = data;

                if ((mapper_info->bank_select_register ^ mapper_info->bank_select_register_previous) & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                    SwapCHRInversion(cartridge);
                }
        
                if ((mapper_info->bank_select_register ^ mapper_info->bank_select_register_previous) & BANK_SELECT_REGISTER_PRG_ROM_MODE_BIT) {
                    SwapPRGROMMode(cartridge);
                }
                break;
            case 0x8001: 
                mapper_info->bank_value_register = data;
                SetPRGCHRBanks(cartridge);
                break;
            case 0xA000: 
                if (cartridge->mirroring != FOUR_SCREEN_MIRRORING) {
                    enum Mirroring mirroring = (data & MIRRORING_BIT) ? HORIZONTAL_MIRRORING : VERTICAL_MIRRORING;
                    CartridgeSetMirroring(cartridge, mirroring);
                }
                break;
            case 0xA001: 
                // originally used in nes for write protecting prg ram to guard saves from corruption caused by power on/off
                break;
            case 0xC000: 
                mapper_info->irq_latch_register = data;
                break;
            case 0xC001: 
                mapper_info->irq_counter_register = 0;
                mapper_info->irq_reload_latch = true;
                break;
            case 0xE000: 
                mapper_info->irq_enabled = false;
                break;
            case 0xE001: 
                mapper_info->irq_enabled = true;;
                break;
        }
    }
}

void Mapper004WritePPU(struct Cartridge* cartridge, uint16_t address, uint8_t data) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    if (cartridge->supports_chr_ram) {
        if (address < 0x0400) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_1_offset] = data;
        } else if (address < 0x0800) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_2_offset] = data;
        } else if (address < 0x0C00) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_3_offset] = data;
        } else if (address < 0x1000) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_4_offset] = data;
        } else if (address < 0x1400) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_5_offset] = data;
        } else if (address < 0x1800) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_6_offset] = data;
        } else if (address < 0x1C00) {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_7_offset] = data;
        } else {
            cartridge->chr_rom[(address & 0x03FF) + mapper_info->chr_rom_bank_8_offset] = data;
        } 
    } else {
        LOG(ERROR, MAPPER, "Attempted write to chr rom\n");
    }
}


bool Mapper004ScanlineIRQ(struct Cartridge* cartridge) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    if ((mapper_info->irq_counter_register == 0) || mapper_info->irq_reload_latch) {
        mapper_info->irq_counter_register = mapper_info->irq_latch_register;
        mapper_info->irq_reload_latch = false;
    } else {
        mapper_info->irq_counter_register--;
    }

    if ((mapper_info->irq_counter_register == 0) && mapper_info->irq_enabled) {
        return true;
    } else {
        return false;
    }
}


static void SetPRGCHRBanks(struct Cartridge* cartridge) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    switch (mapper_info->bank_select_register & BANK_SELECT_REGISTER_BANK_BITS) {
        case 0: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_5_offset = (mapper_info->bank_value_register & 0b11111110) * 0x0400;
                mapper_info->chr_rom_bank_6_offset = ((mapper_info->bank_value_register & 0b11111110) + 1) * 0x0400;
            } else {
                mapper_info->chr_rom_bank_1_offset = (mapper_info->bank_value_register & 0b11111110) * 0x0400;
                mapper_info->chr_rom_bank_2_offset = ((mapper_info->bank_value_register & 0b11111110) + 1) * 0x0400;
            }
            break; 
        case 1: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_7_offset = (mapper_info->bank_value_register & 0b11111110) * 0x0400;
                mapper_info->chr_rom_bank_8_offset = ((mapper_info->bank_value_register & 0b11111110) + 1) * 0x0400;
            } else {
                mapper_info->chr_rom_bank_3_offset = (mapper_info->bank_value_register & 0b11111110) * 0x0400;
                mapper_info->chr_rom_bank_4_offset = ((mapper_info->bank_value_register & 0b11111110) + 1) * 0x0400;
            }
            break; 
        case 2: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_1_offset = mapper_info->bank_value_register * 0x0400;
            } else {
                mapper_info->chr_rom_bank_5_offset = mapper_info->bank_value_register * 0x0400;
            }
            break; 
        case 3: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_2_offset = mapper_info->bank_value_register * 0x0400;
            } else {
                mapper_info->chr_rom_bank_6_offset = mapper_info->bank_value_register * 0x0400;
            }
            break; 
        case 4: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_3_offset = mapper_info->bank_value_register * 0x0400;
            } else {
                mapper_info->chr_rom_bank_7_offset = mapper_info->bank_value_register * 0x0400;
            }
            break; 
        case 5: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_CHR_INVERSION_BIT) {
                mapper_info->chr_rom_bank_4_offset = mapper_info->bank_value_register * 0x0400;
            } else {
                mapper_info->chr_rom_bank_8_offset = mapper_info->bank_value_register * 0x0400;
            }
            break; 
        case 6: 
            if (mapper_info->bank_select_register & BANK_SELECT_REGISTER_PRG_ROM_MODE_BIT) {
                mapper_info->prg_rom_bank_3_offset = (mapper_info->bank_value_register & 0b00111111) * 0x2000;
            } else {
                mapper_info->prg_rom_bank_1_offset = (mapper_info->bank_value_register & 0b00111111) * 0x2000;
            }
            break; 
        case 7: 
            mapper_info->prg_rom_bank_2_offset = (mapper_info->bank_value_register & 0b00111111) * 0x2000;
            break; 
    }
}

static void SwapPRGROMMode(struct Cartridge* cartridge) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    uint32_t temp = mapper_info->prg_rom_bank_1_offset;

    mapper_info->prg_rom_bank_1_offset = mapper_info->prg_rom_bank_3_offset;

    mapper_info->prg_rom_bank_3_offset = temp;
}

static void SwapCHRInversion(struct Cartridge* cartridge) {
    struct Mapper004Info* mapper_info = (struct Mapper004Info*)cartridge->mapper_info;
    uint32_t temp_1 = mapper_info->chr_rom_bank_1_offset;
    uint32_t temp_2 = mapper_info->chr_rom_bank_2_offset;
    uint32_t temp_3 = mapper_info->chr_rom_bank_3_offset;
    uint32_t temp_4 = mapper_info->chr_rom_bank_4_offset;
    
    mapper_info->chr_rom_bank_1_offset = mapper_info->chr_rom_bank_5_offset;
    mapper_info->chr_rom_bank_2_offset = mapper_info->chr_rom_bank_6_offset;
    mapper_info->chr_rom_bank_3_offset = mapper_info->chr_rom_bank_7_offset;
    mapper_info->chr_rom_bank_4_offset = mapper_info->chr_rom_bank_8_offset;
            
    mapper_info->chr_rom_bank_5_offset = temp_1;
    mapper_info->chr_rom_bank_6_offset = temp_2;
    mapper_info->chr_rom_bank_7_offset = temp_3;
    mapper_info->chr_rom_bank_8_offset = temp_4;
}
