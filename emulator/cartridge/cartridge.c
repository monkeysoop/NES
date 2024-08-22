#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cartridge.h"
#include "logger.h"


union Header {
    struct {
    uint8_t name[4];

    uint8_t prg_rom_16KB_units;

    uint8_t chr_rom_8KB_units;

    uint8_t mirroring : 1;
    uint8_t has_battery : 1;
    uint8_t trainer : 1;
    uint8_t alternative_layout : 1;
    uint8_t mapper_id_bits_0123 : 4;

    uint8_t vs_unisystem : 1;
    uint8_t playchoice_10 : 1;
    uint8_t nes_format_id : 2;
    uint8_t mapper_id_bits_4567 : 4;

    uint8_t prg_ram_8KB_units;

    uint8_t TV_system : 1;
    uint8_t reserved : 7;

    uint8_t unused[6];

    };
    uint8_t raw[16];
};




void CartridgeInit(struct Cartridge* cartridge, const char* filename) {
    FILE* cartridge_file = fopen(filename, "r");
    
    if (cartridge_file == NULL) {
        LOG(ERROR, CARTRIDGE, "Failed to open file\n");
    }

    union Header header;
    size_t ret = fread(&header, sizeof(union Header), 1, cartridge_file);
    if (ret != 1) {
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "Failed to read header\n");
    }

    if (header.name[0] != 0x4e || header.name[1] != 0x45 || header.name[2] != 0x53 || header.name[3] != 0x1a) {
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "Incorrect file format\n");
    }

    
    if (header.alternative_layout == 1) {
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "alternative layout not supported\n");
    }


    if (header.nes_format_id & 0b10) {
        // NES 2.0
        cartridge->format = NES_2;
    } else if (header.nes_format_id & 0b01) {
        // Archaic iNES
        cartridge->format = ARCHAIC_iNES;
    } else if (header.nes_format_id == 0b00 && header.raw[12] == 0 && header.raw[13] == 0 
               && header.raw[14] == 0 && header.raw[15] == 0) {
        // iNES
        cartridge->format = iNES;
    } else {
        // iNES 0.7 or archaic iNES
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "format not supported(implemented)\n");
    }


    if (cartridge->format == NES_2) {
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "NES 2.0 (rom format) not supported\n");
    } else if (cartridge->format == ARCHAIC_iNES) {
        fclose(cartridge_file);
        LOG(ERROR, CARTRIDGE, "Archaic iNES (rom format) not supported\n");
    } else if (cartridge->format == iNES) {


        if (header.TV_system == 1) {
            cartridge->tv_system = PAL;
            fclose(cartridge_file);
            LOG(ERROR, CARTRIDGE, "PAL TV system not supported\n");
        } else {
            cartridge->tv_system = NTSC;
        }

        
        cartridge->prg_rom_16KB_units = header.prg_rom_16KB_units;
        cartridge->chr_rom_8KB_units = header.chr_rom_8KB_units;
        cartridge->prg_ram_8KB_units = header.prg_ram_8KB_units;

        if (header.chr_rom_8KB_units == 0) { // 0 means that it's ram not rom and usually with size 8KB
            cartridge->supports_chr_ram = true;
            cartridge->chr_rom_8KB_units = 1;
        } else {
            cartridge->supports_chr_ram = false;
        }

        if (header.prg_ram_8KB_units == 0) {
            cartridge->prg_ram_8KB_units = 1;
        }


        uint8_t mapper_id = header.mapper_id_bits_0123 | (header.mapper_id_bits_4567 << 4);

        LOG(DEBUG_INFO, CARTRIDGE, "mapper id: %d\n", mapper_id);
        LOG(DEBUG_INFO, CARTRIDGE, 
            "prg rom size: %d * 16KB = %dKB\nchr rom size: %d * 8KB = %dKB\nprg ram size: %d * 8KB = %dKB\n", 
            cartridge->prg_rom_16KB_units, (cartridge->prg_rom_16KB_units * 16), 
            cartridge->chr_rom_8KB_units, (cartridge->chr_rom_8KB_units * 8), 
            cartridge->prg_ram_8KB_units, (cartridge->prg_ram_8KB_units * 8)
        );

        cartridge->mapper_id = mapper_id;

        enum Mirroring mirroring = header.mirroring ? VERTICAL_MIRRORING : HORIZONTAL_MIRRORING;
        CartridgeSetMirroring(cartridge, mirroring);

        switch (mapper_id) {
            case NROM: 
                cartridge->mapper_info = malloc(sizeof(struct Mapper000Info));
                Mapper000Init(cartridge); 
                break;
            case SxROM:
                cartridge->mapper_info = malloc(sizeof(struct Mapper001Info));
                Mapper001Init(cartridge); 
                break;
            case UxROM:
                cartridge->mapper_info = malloc(sizeof(struct Mapper002Info));
                Mapper002Init(cartridge); 
                break;
            case CNROM:
                cartridge->mapper_info = malloc(sizeof(struct Mapper003Info));
                Mapper003Init(cartridge); 
                break;
            case MMC3:
                cartridge->mapper_info = malloc(sizeof(struct Mapper004Info));
                Mapper004Init(cartridge); 
                break;
            case AxROM:
            case ColorDreams:
            case GxROM:
            default: 
                fclose(cartridge_file); 
                LOG(ERROR, CARTRIDGE, "Mapper not supported  id: %d\n", mapper_id); 
        }


        // ignore trainer
        if (header.trainer == 1) {
            fseek(cartridge_file, 512, SEEK_CUR);
        }


        size_t prg_rom_size = cartridge->prg_rom_16KB_units * 0x4000;
        size_t chr_rom_size = cartridge->chr_rom_8KB_units * 0x2000;
        size_t prg_ram_size = cartridge->prg_ram_8KB_units * 0x2000;

        cartridge->prg_rom = malloc(prg_rom_size * sizeof(uint8_t));
        cartridge->chr_rom = malloc(chr_rom_size * sizeof(uint8_t));

        memset(cartridge->prg_rom, 0, prg_rom_size * sizeof(uint8_t));
        memset(cartridge->chr_rom, 0, chr_rom_size * sizeof(uint8_t));

        if (fread(cartridge->prg_rom, sizeof(uint8_t), prg_rom_size, cartridge_file) != prg_rom_size) {
            LOG(ERROR, CARTRIDGE, "couldn't read prg rom fully\n");
        } 
        if (!cartridge->supports_chr_ram && fread(cartridge->chr_rom, sizeof(uint8_t), chr_rom_size, cartridge_file) != chr_rom_size) {
            LOG(ERROR, CARTRIDGE, "couldn't read chr rom fully\n");
        }

        if (cartridge->prg_ram_8KB_units != 0) {
            cartridge->prg_ram = malloc(prg_ram_size * sizeof(uint8_t));
            memset(cartridge->prg_ram, 0, prg_ram_size * sizeof(uint8_t));
        }


    }

    fclose(cartridge_file);
    LOG(INFO, CARTRIDGE, "Successfully loaded: %s\n", filename);
    return;
}


void CartridgeClean(struct Cartridge* cartridge) {
    free(cartridge->prg_rom);
    free(cartridge->chr_rom);
    if (cartridge->prg_ram_8KB_units != 0) {
        free(cartridge->prg_ram);
    }
    free(cartridge->mapper_info);
}

bool CartridgeScanlineIRQ(struct Cartridge* cartridge) {
    return cartridge->MapperScanlineIRQ(cartridge);
}

void CartridgeSetMirroring(struct Cartridge* cartridge, enum Mirroring mirroring) {
    cartridge->mirroring = mirroring;
    switch (mirroring) {
        case VERTICAL_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "Vertical Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x400;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case HORIZONTAL_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "Horizontal Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x400;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case ONE_SCREEN_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "One Screen Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        case ONE_SCREEN_LOWER_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "One Screen Lower Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        case ONE_SCREEN_UPPER_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "One Screen Upper Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x400;
            cartridge->mirroring_offsets[1] = 0x400;
            cartridge->mirroring_offsets[2] = 0x400;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case FOUR_SCREEN_MIRRORING: 
            LOG(DEBUG_INFO, CARTRIDGE, "Four Screen Mirroring\n");
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        default:
            LOG(ERROR, CARTRIDGE, "unsuported mirroring\n");
    }
}


uint8_t CartridgeReadCPU(struct Cartridge* cartridge, const uint16_t address) {
    return cartridge->MapperReadCPU(cartridge, address);
}

uint8_t CartridgeReadPPU(struct Cartridge* cartridge, const uint16_t address) {
    return cartridge->MapperReadPPU(cartridge, address);
}


void CartridgeWriteCPU(struct Cartridge* cartridge, const uint16_t address, const uint8_t value) {
    cartridge->MapperWriteCPU(cartridge, address, value);
}

void CartridgeWritePPU(struct Cartridge* cartridge, const uint16_t address, const uint8_t value) {
    cartridge->MapperWritePPU(cartridge, address, value);
}
