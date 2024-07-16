#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>
#include <stdlib.h>

typedef enum {
    NROM  = 0,
    SxROM = 1,
    UxROM = 2,
    CNROM = 3,
    MMC3  = 4,
    AxROM = 7,
    ColorDreams = 11,
    GxROM = 66,
} MapperType;




typedef struct Mapper {
    MapperType mapper_type;

    uint8_t prg_rom_16KB_units;
    uint8_t* prg_rom;

    uint8_t chr_rom_8KB_units;
    uint8_t* chr_rom;

    uint16_t cpu_address_mask;
    uint16_t ppu_address_mask;

    uint8_t (*MapperReadCPU)(struct Mapper*, uint16_t);
    uint8_t (*MapperReadPPU)(struct Mapper*, uint16_t);
    
    void (*MapperWriteCPU)(struct Mapper*, uint16_t, uint8_t);
    void (*MapperWritePPU)(struct Mapper*, uint16_t, uint8_t);

    void (*MapperClean)(struct Mapper*);
} Mapper;


void Mapper000Init(Mapper* mapper, uint8_t prg_rom_16KB_units, uint8_t chr_rom_8KB_units);
void Mapper001Init(Mapper* mapper, uint8_t prg_rom_16KB_units, uint8_t chr_rom_8KB_units);
void Mapper002Init(Mapper* mapper, uint8_t prg_rom_16KB_units, uint8_t chr_rom_8KB_units);
void Mapper003Init(Mapper* mapper, uint8_t prg_rom_16KB_units, uint8_t chr_rom_8KB_units);



#endif