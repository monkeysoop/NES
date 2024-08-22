#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>

#include "ppu_bus.h"

static const uint32_t nes_palette_colors_rgba[64] = {
    0x666666FF, 0x002A88FF, 0x1412A7FF, 0x3B00A4FF, 0x5C007EFF, 0x6E0040FF, 0x6C0600FF, 0x561D00FF,
    0x333500FF, 0x0B4800FF, 0x005200FF, 0x004F08FF, 0x00404DFF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xADADADFF, 0x155FD9FF, 0x4240FFFF, 0x7527FEFF, 0xA01ACCFF, 0xB71E7BFF, 0xB53120FF, 0x994E00FF,
    0x6B6D00FF, 0x388700FF, 0x0C9300FF, 0x008F32FF, 0x007C8DFF, 0x000000FF, 0x000000FF, 0x000000FF,
    0xFFFEFFFF, 0x64B0FFFF, 0x9290FFFF, 0xC676FFFF, 0xF36AFFFF, 0xFE6ECCFF, 0xFE8170FF, 0xEA9E22FF,
    0xBCBE00FF, 0x88D800FF, 0x5CE430FF, 0x45E082FF, 0x48CDDEFF, 0x4F4F4FFF, 0x000000FF, 0x000000FF,
    0xFFFEFFFF, 0xC0DFFFFF, 0xD3D2FFFF, 0xE8C8FFFF, 0xFBC2FFFF, 0xFEC4EAFF, 0xFECCC5FF, 0xF7D8A5FF,
    0xE4E594FF, 0xCFEF96FF, 0xBDF4ABFF, 0xB3F3CCFF, 0xB5EBF2FF, 0xB8B8B8FF, 0x000000FF, 0x000000FF,
};


#define PALETTE_BUFFER_WIDTH 4
#define PALETTE_BUFFER_HEIGHT 8

#define PATTERN_TABLE_WIDTH 128
#define PATTERN_TABLE_HEIGHT 128

#define NAMETABLE_BYTE_WIDTH 3

#define NAMETABLE_BYTE_BUFFER_WIDTH 32
#define NAMETABLE_BYTE_BUFFER_HEIGHT 32

// ctrl register
#define BASE_NAMETABLE_BITS                  0b00000011
#define VRAM_ADDRESS_INCREMENT_BIT           0b00000100
#define SPRITE_PATTERN_TABLE_ADDRESS_BIT     0b00001000
#define BACKGROUND_PATTERN_TABLE_ADDRESS_BIT 0b00010000
#define SPRITE_SIZE_BIT                      0b00100000
#define MASTER_SLAVE_SELECT_BIT              0b01000000
#define GENERATE_NMI_BIT                     0b10000000

// mask register
#define GREYSCALE_BIT                0b00000001
#define SHOW_BACKGROUND_LEFTMOST_BIT 0b00000010
#define SHOW_SPRITES_LEFTMOST_BIT    0b00000100
#define SHOW_BACKGROUND_BIT          0b00001000
#define SHOW_SPRITES_BIT             0b00010000
#define TINT_RED_GREEN_ON_PAL_BIT    0b00100000
#define TINT_GREEN_RED_ON_PAL_BIT    0b01000000
#define TINT_BLUE_BIT                0b10000000

// status register
#define STALE_PPU_BUS_CONTENTS_BITS 0b00011111
#define SPRITE_OVERFLOW_BIT         0b00100000
#define SPRITE_ZERO_HIT_BIT         0b01000000
#define VERTICAL_BLANK_BIT          0b10000000

// sprite attributes
#define SPRITE_PALETTE_BITS          0b00000011
#define SPRITE_UNUSED_BITS           0b00011100
#define SPRITE_PRIORITY_BIT          0b00100000
#define FLIP_SPRITE_HORIZONTALLY_BIT 0b01000000
#define FLIP_SPRITE_VERTICALLY_BIT   0b10000000



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


#define NES_SCREEN_WIDTH 256
#define NES_SCREEN_HEIGHT 240

enum RenderState {
    RENDER,
    POST_RENDER,
    VERTICAL_BLANKING,
    PRE_RENDER,
    FINISHED,
};

enum GenerateInterrupt {
    GENERATE_NMI,
    GENERATE_IRQ,
    GENERATE_NO_INTERRUPT,
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
    uint8_t scanline_OAM_indecies[8]; 
    uint8_t scanline_OAM_length;

    enum RenderState render_state;
    uint16_t scanline;
    uint16_t cycle;

    uint8_t is_odd_frame;
    
    bool sprite_0_hit_happened; 

    struct PPUBus* ppu_bus;
};

void PPUInit(struct PPU* ppu, struct PPUBus* ppu_bus, enum TVSystem tv_system);
void PPUReset(struct PPU* ppu, enum TVSystem tv_system);

enum GenerateInterrupt PPUClockNTSC(struct PPU* ppu, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]);
enum GenerateInterrupt PPUClockPAL(struct PPU* ppu, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]);

void DebugView(
    struct PPU* ppu, 
    uint8_t palette_buffer[PALETTE_BUFFER_HEIGHT][PALETTE_BUFFER_WIDTH], 
    uint32_t pattern_tables_pixels_buffer[2][PATTERN_TABLE_WIDTH * PATTERN_TABLE_HEIGHT],
    uint8_t selected_palette,
    char nametable_buffer[NAMETABLE_BYTE_BUFFER_HEIGHT][NAMETABLE_BYTE_BUFFER_WIDTH * NAMETABLE_BYTE_WIDTH],
    uint8_t selected_nametable
);

void PPUWriteCtrl(struct PPU* ppu, const uint8_t data);
void PPUWriteMask(struct PPU* ppu, const uint8_t data);
void PPUWriteOAMAddress(struct PPU* ppu, const uint8_t data);
void PPUWriteOAMData(struct PPU* ppu, const uint8_t data);
void PPUWriteScroll(struct PPU* ppu, const uint8_t data);
void PPUWritePPUAddress(struct PPU* ppu, const uint8_t data);
void PPUWritePPUData(struct PPU* ppu, const uint8_t data);

uint8_t PPUReadStatus(struct PPU* ppu);
uint8_t PPUReadOAMData(struct PPU* ppu);
uint8_t PPUReadPPUData(struct PPU* ppu);



#endif