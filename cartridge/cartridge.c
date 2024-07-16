#include "cartridge.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
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

} Header;




Cartridge* CartridgeInit(Cartridge* cartridge, const char* filename) {
    FILE* cartridge_file = fopen(filename, "r");
    
    if (cartridge_file == NULL) {
        printf("Failed to open file\n");
        return NULL;
    }

    Header header;
    size_t ret = fread(&header, sizeof(Header), 1, cartridge_file);
    if (ret != 1) {
        printf("Failed to read header\n");
        fclose(cartridge_file);
        return NULL;
    }

    if (header.name[0] != 0x4e || header.name[0] != 0x45 || header.name[0] != 0x53 || header.name[0] != 0x1a) {
        printf("Incorrect file format\n");
        fclose(cartridge_file);
        return NULL;
    }



    
    if (header.alternative_layout == 1) {
        printf("alternative layout not supported\n");
        fclose(cartridge_file);
        return NULL;
    }




    if (header.nes_format_id & 0b10) {
        // NES 2.0
        cartridge->format = NES_2;
    } else if (header.nes_format_id & 0b01) {
        // Archaic iNES
        cartridge->format = ARCHAIC_iNES;
    } else if (header.nes_format_id == 0b00 && header.unused[12] == 0 && header.unused[13] == 0 
               && header.unused[14] == 0 && header.unused[15] == 0) {
        // iNES
        cartridge->format = iNES;
    } else {
        // iNES 0.7 or archaic iNES
        printf("format not supported(implemented)\n");
        fclose(cartridge_file);
        return NULL;
    }



    if (cartridge->format == NES_2) {
        printf("NES 2.0 (rom format) not supported\n");
        fclose(cartridge_file);
        return NULL;
    } else if (cartridge->format == ARCHAIC_iNES) {
        printf("Archaic iNES (rom format) not supported\n");
        fclose(cartridge_file);
        return NULL;
    } else if (cartridge->format == iNES) {
        cartridge->nametable_layout = header.mirroring ? VERTICAL : HORIZONTAL;

        if (header.TV_system == 1) {
            cartridge->tv_system = PAL;
        } else {
            cartridge->tv_system = NTSC;
        }

        
        uint8_t mapper_id = header.mapper_id_bits_0123 | (header.mapper_id_bits_4567 << 4);
    

        if (header.chr_rom_8KB_units == 0) {
            // 0 means that it's ram not rom and usually with size 8KB
            printf("CHR RAM not supported\n");
            fclose(cartridge_file);
            return NULL;
        }

        switch (mapper_id) {
            case NROM: Mapper000Init(cartridge->mapper, header.prg_rom_16KB_units, header.chr_rom_8KB_units); break;
            default: 
                printf("Mapper not supported\n"); 
                fclose(cartridge_file); 
                return NULL;
        }


        // ignore trainer
        if (header.trainer == 1) {
            fseek(cartridge_file, 512, SEEK_CUR);
        }

        unsigned int prg_rom_size = header.prg_rom_16KB_units * 16 * 1024;
        unsigned int chr_rom_size = header.chr_rom_8KB_units * 8 * 1024;

        fread(cartridge->mapper->prg_rom, sizeof(uint8_t), prg_rom_size, cartridge_file);
        fread(cartridge->mapper->chr_rom, sizeof(uint8_t), chr_rom_size, cartridge_file);
    }




    fclose(cartridge_file);
}


void CartridgeClean(Cartridge* cartridge) {
    cartridge->mapper->MapperClean(cartridge->mapper);
}


uint8_t CartridgeReadCPU(Cartridge* cartridge, const uint16_t address) {
    return cartridge->mapper->MapperReadCPU(cartridge->mapper, address);
}

uint8_t CartridgeReadPPU(Cartridge* cartridge, const uint16_t address) {
    return cartridge->mapper->MapperReadPPU(cartridge->mapper, address);
}


void CartridgeWriteCPU(Cartridge* cartridge, const uint16_t address, const uint8_t value) {
    cartridge->mapper->MapperWriteCPU(cartridge->mapper, address, value);
}

void CartridgeWritePPU(Cartridge* cartridge, const uint16_t address, const uint8_t value) {
    cartridge->mapper->MapperWritePPU(cartridge->mapper, address, value);
}
