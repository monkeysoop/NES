#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <string.h>
#include "ppu_bus.h"
#include "cpu.h"

#define BASE_NAMETABLE_BITS                  0b00000011
#define VRAM_ADDRESS_INCREMENT_BIT           0b00000100
#define SPRITE_PATTERN_TABLE_ADDRESS_BIT     0b00001000
#define BACKGROUND_PATTERN_TABLE_ADDRESS_BIT 0b00010000
#define SPRITE_SIZE_BIT                      0b00100000
#define MASTER_SLAVE_SELECT_BIT              0b01000000
#define GENERATE_NMI_BIT                     0b10000000

#define GREYSCALE_BIT                0b00000001
#define SHOW_BACKGROUND_LEFTMOST_BIT 0b00000010
#define SHOW_SPRITES_LEFTMOST_BIT    0b00000100
#define SHOW_BACKGROUND_BIT          0b00001000
#define SHOW_SPRITES_BIT             0b00010000
#define TINT_RED_GREEN_ON_PAL_BIT    0b00100000
#define TINT_GREEN_RED_ON_PAL_BIT    0b01000000
#define TINT_BLUE_BIT                0b10000000

#define STALE_PPU_BUS_CONTENTS_BITS 0b00011111
#define SPRITE_OVERFLOW_BIT         0b00100000
#define SPRITE_ZERO_HIT_BIT         0b01000000
#define VERTICAL_BLANK_BIT          0b10000000



#define RENDER_SCANLINE_END 240

#define NTSC_POST_RENDER_SCANLINE_END 241
#define PAL_POST_RENDER_SCANLINE_END 241
#define DENDY_POST_RENDER_SCANLINE_END 291

#define NTSC_VERTICAL_BLANKING_SCANLINE_END 261
#define PAL_VERTICAL_BLANKING_SCANLINE_END 311
#define DENDY_VERTICAL_BLANKING_SCANLINE_END 311

#define NTSC_PRE_RENDER_SCANLINE_END 262
#define PAL_PRE_RENDER_SCANLINE_END 312
#define DENDY_PRE_RENDER_SCANLINE_END 312


#define SCANLINE_DOTS 341
#define SCANLINE_VISIBLE_DOTS 256
#define SCANLINE_IRQ_CYCLE 260
#define SCANLINE_LAST_CYCLE 340

typedef enum RenderState {
    RENDER,
    POST_RENDER,
    VERTICAL_BLANKING,
    PRE_RENDER,
    FINISHED,
} RenderState;



typedef struct PPU {
    uint8_t ctrl_register;
    uint8_t mask_register;
    uint8_t status_register;
    uint8_t oam_address_register;
    uint8_t oam_data_register;
    uint8_t scroll_register;
    uint8_t ppu_address_register;
    uint8_t ppu_data_register;

    // internal registers
    uint16_t v;
    uint16_t t;
    uint8_t x;
    uint8_t w;

    uint8_t ppu_data_buffer;

    uint8_t OAM[256];

    RenderState render_state;
    uint16_t scanline;
    uint16_t cycle;

    uint8_t is_odd_frame;

    PPUBus* ppu_bus;
    CPU* cpu;
} PPU;

void PPUInit(PPU* ppu, PPUBus* ppu_bus, CPU* cpu, TVSystem tv_system);
void PPUReset(PPU* ppu, TVSystem tv_system);

void PPUClockNTSC(PPU* ppu);
void PPUClockPAL(PPU* ppu);


void PPUWriteCtrl(PPU* ppu, const uint8_t data);
void PPUWriteMask(PPU* ppu, const uint8_t data);
void PPUWriteOAMAddress(PPU* ppu, const uint8_t data);
void PPUWriteOAMData(PPU* ppu, const uint8_t data);
void PPUWriteScroll(PPU* ppu, const uint8_t data);
void PPUWritePPUAddress(PPU* ppu, const uint8_t data);
void PPUWritePPUData(PPU* ppu, const uint8_t data);
void PPUWriteDMAAddress(PPU* ppu, const uint8_t data);

uint8_t PPUReadStatus(PPU* ppu);
uint8_t PPUReadOAMData(PPU* ppu);
uint8_t PPUReadPPUData(PPU* ppu);



#endif