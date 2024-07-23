#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>
#include <stdlib.h>

#define NROM 0
#define SxROM 1
#define UxROM 2
#define CNROM 3
#define MMC3 4
#define AxROM 7
#define ColorDreams 11
#define GxROM 66

typedef enum FileFormat {
    iNES,
    NES_2,
    ARCHAIC_iNES,
} FileFormat;

typedef enum TVSystem {
    PAL,
    NTSC,
} TVSystem;


typedef enum Mirroring {
    VERTICAL_MIRRORING,
    HORIZONTAL_MIRRORING,
    ONE_SCREEN_MIRRORING,
    ONE_SCREEN_LOWER_MIRRORING,
    ONE_SCREEN_UPPER_MIRRORING,
    FOUR_SCREEN_MIRRORING,
} Mirroring;

typedef struct Cartridge {
    FileFormat format;
    TVSystem tv_system;
    Mirroring mirroring;
    uint8_t mapper_id;

    uint8_t prg_rom_16KB_units;
    uint8_t prg_ram_8KB_units;
    uint8_t chr_rom_8KB_units;

    uint8_t* prg_rom;
    uint8_t* prg_ram;
    uint8_t* chr_rom;

    uint16_t cpu_address_mask;
    uint16_t ppu_address_mask;

    uint16_t mirroring_offsets[4];

    uint8_t (*MapperReadCPU)(struct Cartridge*, uint16_t);
    uint8_t (*MapperReadPPU)(struct Cartridge*, uint16_t);
    
    void (*MapperWriteCPU)(struct Cartridge*, uint16_t, uint8_t);
    void (*MapperWritePPU)(struct Cartridge*, uint16_t, uint8_t);

    void (*MapperScanlineIRQ)(struct Cartridge*);
} Cartridge;

void CartridgeInit(Cartridge* cartridge, const char* filename);
void CartridgeClean(Cartridge* cartridge);

void CartridgeScanlineIRQ(Cartridge* cartridge);
void CartridgeSetMirroring(Cartridge* cartridge, Mirroring mirroring);

uint8_t CartridgeReadCPU(Cartridge* cartridge, const uint16_t address);
uint8_t CartridgeReadPPU(Cartridge* cartridge, const uint16_t address);
void CartridgeWriteCPU(Cartridge* cartridge, const uint16_t address, const uint8_t value);
void CartridgeWritePPU(Cartridge* cartridge, const uint16_t address, const uint8_t value);


void Mapper000Init(Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);
void Mapper001Init(Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);
void Mapper002Init(Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);
void Mapper003Init(Cartridge* cartridge, uint8_t prg_rom_16KB_units, uint8_t prg_ram_8KB_units, uint8_t chr_rom_8KB_units);

#endif