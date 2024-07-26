#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>

#include "ppu_bus.h"

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

enum RenderState {
    RENDER,
    POST_RENDER,
    VERTICAL_BLANKING,
    PRE_RENDER,
    FINISHED,
};



struct PPU {
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

    enum RenderState render_state;
    uint16_t scanline;
    uint16_t cycle;

    uint8_t is_odd_frame;

    struct PPUBus* ppu_bus;
};

void PPUInit(struct PPU* ppu, struct PPUBus* ppu_bus, enum TVSystem tv_system);
void PPUReset(struct PPU* ppu, enum TVSystem tv_system);

bool PPUClockNTSC(struct PPU* ppu);
bool PPUClockPAL(struct PPU* ppu);


void PPUWriteCtrl(struct PPU* ppu, const uint8_t data);
void PPUWriteMask(struct PPU* ppu, const uint8_t data);
void PPUWriteOAMAddress(struct PPU* ppu, const uint8_t data);
void PPUWriteOAMData(struct PPU* ppu, const uint8_t data);
void PPUWriteScroll(struct PPU* ppu, const uint8_t data);
void PPUWritePPUAddress(struct PPU* ppu, const uint8_t data);
void PPUWritePPUData(struct PPU* ppu, const uint8_t data);
void PPUWriteDMAAddress(struct PPU* ppu, const uint8_t data);

uint8_t PPUReadStatus(struct PPU* ppu);
uint8_t PPUReadOAMData(struct PPU* ppu);
uint8_t PPUReadPPUData(struct PPU* ppu);



#endif