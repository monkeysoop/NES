#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define NROM 0
#define SxROM 1
#define UxROM 2
#define CNROM 3
#define MMC3 4
#define AxROM 7
#define ColorDreams 11
#define GxROM 66

enum FileFormat {
    iNES,
    NES_2,
    ARCHAIC_iNES,
};

enum TVSystem {
    PAL,
    NTSC,
};


enum Mirroring {
    VERTICAL_MIRRORING,
    HORIZONTAL_MIRRORING,
    ONE_SCREEN_MIRRORING,
    ONE_SCREEN_LOWER_MIRRORING,
    ONE_SCREEN_UPPER_MIRRORING,
    FOUR_SCREEN_MIRRORING,
};

struct Cartridge {
    enum FileFormat format;
    enum TVSystem tv_system;
    enum Mirroring mirroring;
    uint8_t mapper_id;

    uint8_t prg_rom_16KB_units;
    uint8_t prg_ram_8KB_units;
    uint8_t chr_rom_8KB_units;

    bool supports_chr_ram;

    uint8_t* prg_rom;
    uint8_t* prg_ram;
    uint8_t* chr_rom;

    void* mapper_info;

    uint16_t mirroring_offsets[4];

    uint8_t (*MapperReadCPU)(struct Cartridge*, uint16_t);
    uint8_t (*MapperReadPPU)(struct Cartridge*, uint16_t);
    
    void (*MapperWriteCPU)(struct Cartridge*, uint16_t, uint8_t);
    void (*MapperWritePPU)(struct Cartridge*, uint16_t, uint8_t);

    void (*MapperScanlineIRQ)(struct Cartridge*);
};

struct Mapper000Info {
    uint16_t cpu_address_mask;
};

void CartridgeInit(struct Cartridge* cartridge, const char* filename);
void CartridgeClean(struct Cartridge* cartridge);

void CartridgeScanlineIRQ(struct Cartridge* cartridge);
void CartridgeSetMirroring(struct Cartridge* cartridge, enum Mirroring mirroring);

uint8_t CartridgeReadCPU(struct Cartridge* cartridge, const uint16_t address);
uint8_t CartridgeReadPPU(struct Cartridge* cartridge, const uint16_t address);
void CartridgeWriteCPU(struct Cartridge* cartridge, const uint16_t address, const uint8_t value);
void CartridgeWritePPU(struct Cartridge* cartridge, const uint16_t address, const uint8_t value);


void Mapper000Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units);
void Mapper001Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);
void Mapper002Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);
void Mapper003Init(struct Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);

#endif