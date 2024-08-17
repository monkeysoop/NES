#include "cartridge.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
        printf("Failed to open file\n");
        exit(1);
    }

    union Header header;
    size_t ret = fread(&header, sizeof(union Header), 1, cartridge_file);
    if (ret != 1) {
        printf("Failed to read header\n");
        fclose(cartridge_file);
        exit(1);
    }

    if (header.name[0] != 0x4e || header.name[1] != 0x45 || header.name[2] != 0x53 || header.name[3] != 0x1a) {
        printf("Incorrect file format\n");
        fclose(cartridge_file);
        exit(1);
    }

    
    if (header.alternative_layout == 1) {
        printf("alternative layout not supported\n");
        fclose(cartridge_file);
        exit(1);
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
        printf("format not supported(implemented)\n");
        fclose(cartridge_file);
        exit(1);
    }


    if (cartridge->format == NES_2) {
        printf("NES 2.0 (rom format) not supported\n");
        fclose(cartridge_file);
        exit(1);
    } else if (cartridge->format == ARCHAIC_iNES) {
        printf("Archaic iNES (rom format) not supported\n");
        fclose(cartridge_file);
        exit(1);
    } else if (cartridge->format == iNES) {


        if (header.TV_system == 1) {
            cartridge->tv_system = PAL;
            printf("PAL TV system not supported\n");
            fclose(cartridge_file);
            exit(1);
        } else {
            cartridge->tv_system = NTSC;
        }

        
        cartridge->prg_rom_16KB_units = header.prg_rom_16KB_units;
        cartridge->chr_rom_8KB_units= header.chr_rom_8KB_units;
        cartridge->prg_ram_8KB_units = header.prg_ram_8KB_units;

        if (header.chr_rom_8KB_units == 0) { // 0 means that it's ram not rom and usually with size 8KB
            cartridge->supports_chr_ram = true;
            cartridge->chr_rom_8KB_units = 1;
        } else {
            cartridge->supports_chr_ram = false;
        }


        uint8_t mapper_id = header.mapper_id_bits_0123 | (header.mapper_id_bits_4567 << 4);

        cartridge->mapper_id = mapper_id;

        switch (mapper_id) {
            case NROM: 
                cartridge->mapper_info = malloc(sizeof(struct Mapper000Info));
                Mapper000Init(cartridge, cartridge->prg_rom_16KB_units); break;
            case SxROM:
            case UxROM:
            case CNROM:
            case MMC3:
            case AxROM:
            case ColorDreams:
            case GxROM:
            default: 
                printf("Mapper not supported  id: %d\n", mapper_id); 
                fclose(cartridge_file); 
                exit(1);
        }


        // ignore trainer
        if (header.trainer == 1) {
            fseek(cartridge_file, 512, SEEK_CUR);
        }


        unsigned int prg_rom_size = cartridge->prg_rom_16KB_units * 16 * 1024;
        unsigned int chr_rom_size = cartridge->chr_rom_8KB_units * 8 * 1024;
        unsigned int prg_ram_size = cartridge->prg_ram_8KB_units * 8 * 1024;

        cartridge->prg_rom = malloc(prg_rom_size * sizeof(uint8_t));
        cartridge->chr_rom = malloc(chr_rom_size * sizeof(uint8_t));

        memset(cartridge->prg_rom, 0, prg_rom_size * sizeof(uint8_t));
        memset(cartridge->chr_rom, 0, chr_rom_size * sizeof(uint8_t));

        fread(cartridge->prg_rom, sizeof(uint8_t), prg_rom_size, cartridge_file);
        fread(cartridge->chr_rom, sizeof(uint8_t), chr_rom_size, cartridge_file);


        if (cartridge->prg_ram_8KB_units != 0) {
            cartridge->prg_ram = malloc(prg_ram_size * sizeof(uint8_t));
            memset(cartridge->prg_ram, 0, prg_ram_size * sizeof(uint8_t));
        }


        enum Mirroring mirroring = header.mirroring ? VERTICAL_MIRRORING : HORIZONTAL_MIRRORING;
        CartridgeSetMirroring(cartridge, mirroring);
    }

    fclose(cartridge_file);
    printf("Successfully loaded: %s\n", filename);
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

void CartridgeScanlineIRQ(struct Cartridge* cartridge) {
    if (cartridge->mapper_id == MMC3) {
        cartridge->MapperScanlineIRQ(cartridge);
    }
}

void CartridgeSetMirroring(struct Cartridge* cartridge, enum Mirroring mirroring) {
    switch (mirroring) {
        case VERTICAL_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x400;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case HORIZONTAL_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x400;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case ONE_SCREEN_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        case ONE_SCREEN_LOWER_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        case ONE_SCREEN_UPPER_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x400;
            cartridge->mirroring_offsets[1] = 0x400;
            cartridge->mirroring_offsets[2] = 0x400;
            cartridge->mirroring_offsets[3] = 0x400;
            break;
        case FOUR_SCREEN_MIRRORING: 
            cartridge->mirroring_offsets[0] = 0x0;
            cartridge->mirroring_offsets[1] = 0x0;
            cartridge->mirroring_offsets[2] = 0x0;
            cartridge->mirroring_offsets[3] = 0x0;
            break;
        default:
            printf("unsuported mirroring\n");
            exit(1);
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
