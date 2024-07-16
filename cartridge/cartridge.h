#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>
#include <stdio.h>
#include "mapper.h"


typedef enum {
    NO_MIRRORING,
    VERTICAL,
    HORIZONTAL,
    ONE_SCREEN,
    ONE_SCREEN_LOWER,
    ONE_SCREEN_UPPER,
    FOUR_SCREEN,
} NametableLayout;

typedef enum {
    iNES,
    NES_2,
    ARCHAIC_iNES,
} Format;

typedef enum {
    PAL,
    NTSC,
} TVSystem;

typedef struct {
    NametableLayout nametable_layout;

    Format format;
    TVSystem tv_system;

    Mapper* mapper;
} Cartridge;

Cartridge* CartridgeInit(Cartridge* cartridge, const char* filename);
void CartridgeClean(Cartridge* cartridge);

uint8_t CartridgeReadCPU(Cartridge* cartridge, const uint16_t address);
uint8_t CartridgeReadPPU(Cartridge* cartridge, const uint16_t address);
void CartridgeWriteCPU(Cartridge* cartridge, const uint16_t address, const uint8_t value);
void CartridgeWritePPU(Cartridge* cartridge, const uint16_t address, const uint8_t value);


#endif