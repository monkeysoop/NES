#include "mapper.h"


void Mapper000Init(Mapper* mapper, uint8_t prg_rom_16KB_units, uint8_t chr_rom_8KB_units) {
    size_t prg_rom_size = prg_rom_16KB_units * 16 * 1024 * sizeof(uint8_t);
    size_t chr_rom_size = chr_rom_8KB_units * 16 * 1024 * sizeof(uint8_t);

    if (prg_rom_16KB_units == 1) {
        mapper->cpu_address_mask = 0x7FFF;
    } else {
        mapper->cpu_address_mask = 0x3FFF;
    }

    mapper->prg_rom = malloc(prg_rom_size);
    mapper->chr_rom = malloc(chr_rom_size);

    memset(mapper->prg_rom, 0, prg_rom_size);
    memset(mapper->chr_rom, 0, chr_rom_size);

    mapper->MapperReadCPU = &MapperReadCPU;
    mapper->MapperReadPPU = &MapperReadPPU;
    mapper->MapperWriteCPU = &MapperWriteCPU;
    mapper->MapperWritePPU = &MapperWritePPU;

    mapper->MapperClean = &MapperClean;
}


void MapperClean(Mapper* mapper) {
    free(mapper->prg_rom);
    free(mapper->chr_rom);
}

uint8_t MapperReadCPU(struct Mapper* mapper, uint16_t address) {
    return (mapper->prg_rom[(address - 0x8000) & mapper->cpu_address_mask]);
}

uint8_t MapperReadPPU(struct Mapper* mapper, uint16_t address) {
    return mapper->chr_rom[address];
}


void MapperWriteCPU(struct Mapper* mapper, uint16_t address, uint8_t data) {
    printf("Attempted write to prg rom on mapper 000\n");
    exit(1);
}

void MapperWritePPU(struct Mapper* mapper, uint16_t address, uint8_t data) {
    printf("Attempted write to chr rom on mapper 000 (some games can have chr rams but it's not supported)\n");
    exit(1);
}
