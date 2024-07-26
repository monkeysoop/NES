#include <stdio.h>
#include <string.h>

#include "ppu.h"

static const uint32_t nes_palette_colors_rgba[64] = {
    0xff666666, 0xff002a88, 0xff1412a7, 0xff3b00a4, 0xff5c007e, 0xff6e0040, 0xff6c0600, 0xff561d00,
    0xff333500, 0xff0b4800, 0xff005200, 0xff004f08, 0xff00404d, 0xff000000, 0xff000000, 0xff000000,
    0xffadadad, 0xff155fd9, 0xff4240ff, 0xff7527fe, 0xffa01acc, 0xffb71e7b, 0xffb53120, 0xff994e00,
    0xff6b6d00, 0xff388700, 0xff0c9300, 0xff008f32, 0xff007c8d, 0xff000000, 0xff000000, 0xff000000,
    0xfffffeff, 0xff64b0ff, 0xff9290ff, 0xffc676ff, 0xfff36aff, 0xfffe6ecc, 0xfffe8170, 0xffea9e22,
    0xffbcbe00, 0xff88d800, 0xff5ce430, 0xff45e082, 0xff48cdde, 0xff4f4f4f, 0xff000000, 0xff000000,
    0xfffffeff, 0xffc0dfff, 0xffd3d2ff, 0xffe8c8ff, 0xfffbc2ff, 0xfffec4ea, 0xfffeccc5, 0xfff7d8a5,
    0xffe4e594, 0xffcfef96, 0xffbdf4ab, 0xffb3f3cc, 0xffb5ebf2, 0xffb8b8b8, 0xff000000, 0xff000000,
};



void PPUInit(struct PPU* ppu, struct PPUBus* ppu_bus, enum TVSystem tv_system) {
    ppu->ctrl_register = 0;
    ppu->mask_register = 0;
    ppu->status_register = 0b10100000; // wiki says these bits are often set
    ppu->oam_address_register = 0;
    ppu->oam_data_register = 0;
    ppu->scroll_register = 0;
    ppu->ppu_address_register = 0;
    ppu->ppu_data_register = 0;

    ppu->v = 0;
    ppu->t = 0;
    ppu->x = 0;
    ppu->w = 0;

    ppu->ppu_data_buffer = 0;

    memset(ppu->OAM, 0, 256 * sizeof(uint8_t));

    ppu->render_state = PRE_RENDER;
    ppu->scanline = (tv_system == NTSC) ? NTSC_VERTICAL_BLANKING_SCANLINE_END : PAL_VERTICAL_BLANKING_SCANLINE_END;
    ppu->cycle = 0;

    ppu->is_odd_frame = 0;

    ppu->ppu_bus = ppu_bus;
}

void PPUReset(struct PPU* ppu, enum TVSystem tv_system) {
    ppu->ctrl_register = 0;
    ppu->mask_register = 0;
    ppu->status_register = ppu->status_register & 0b10000000;
    ppu->scroll_register = 0;
    ppu->ppu_data_register = 0;

    ppu->v = 0;
    ppu->t = 0;
    ppu->t = 0;
    ppu->w = 0;

    ppu->ppu_data_buffer = 0;

    memset(ppu->OAM, 0, 256 * sizeof(uint8_t));

    ppu->render_state = PRE_RENDER;
    ppu->scanline = (tv_system == NTSC) ? NTSC_VERTICAL_BLANKING_SCANLINE_END : PAL_VERTICAL_BLANKING_SCANLINE_END;
    ppu->cycle = 0;

    ppu->is_odd_frame = 0;
}


bool PPUClockNTSC(struct PPU* ppu) {
    bool interrupt_cpu = false;
    switch (ppu->render_state) {
        case RENDER: 
            if (ppu->cycle > 0 && ppu->cycle <= SCANLINE_VISIBLE_DOTS) {
                uint8_t dot = ppu->cycle - 1;   // dot is basically x
                uint8_t x_fine = (ppu->x + dot) & 0x07;
                
                uint8_t background_color_address = 0;
                uint8_t background_opaque = 0;

                uint8_t sprite_color_address = 0;
                uint8_t sprite_opaque = 0;

                uint8_t sprite_in_foreground = 0;


                if (ppu->mask_register & SHOW_BACKGROUND_BIT) {
                    if ((ppu->mask_register & SHOW_BACKGROUND_LEFTMOST_BIT) || dot >= 8) {
                        // render background
                        uint16_t tile_address = 0x2000 | (ppu->v & 0x0FFF);

                        uint16_t pattern_address = (((uint16_t)PPUBusRead(ppu->ppu_bus, tile_address) >> 4) + ((ppu->v >> 12) & 0x0007)) 
                                                 | ((ppu->ctrl_register & BACKGROUND_PATTERN_TABLE_ADDRESS_BIT) << 8);
                        uint16_t attribute_address = 0x23C0 | (ppu->v & 0x0C00) | ((ppu->v >> 4) & 0x0038) | ((ppu->v >> 2) & 0x0007);

                        background_color_address = ((PPUBusRead(ppu->ppu_bus, pattern_address) >> (x_fine ^ 0x07)) & 0x01)
                                                 | ((PPUBusRead(ppu->ppu_bus, (pattern_address + 8)) >> ((x_fine ^ 0x07) - 1)) & 0x02);
                        
                        background_opaque = background_color_address;

                        background_color_address |= (((PPUBusRead(ppu->ppu_bus, attribute_address) >> (((ppu->v >> 4) & 0x04) | (ppu->v & 0x02))) & 0x03) << 2);
                    }

                    if (x_fine == 7) {
                        if ((ppu->v & 0x001F) == 0x001F) {
                            ppu->v &= ~0x001F;
                            ppu->v ^= 0x0400;
                        } else {
                            ppu->v++;
                        }
                    }
                }
                if ((ppu->mask_register & SHOW_SPRITES_BIT) && ((ppu->mask_register & SHOW_SPRITES_LEFTMOST_BIT) || dot >= 8)) {
                    // TODO: render sprites
                }


                uint8_t color_address = background_color_address;
                if (sprite_opaque && (!background_opaque || sprite_in_foreground)) {
                    color_address = sprite_color_address;
                }

                nes_palette_colors_rgba[PPUBusRead(ppu->ppu_bus, (0x3F00 + color_address))];
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 1)) && (ppu->mask_register & SHOW_BACKGROUND_BIT)) {
                if ((ppu->v & 0x7000) != 0x7000) {
                    ppu->v += 0x1000;
                } else {
                    ppu->v &= ~0x7000;
                    uint16_t y_coarse = (ppu->v & 0x03E0) >> 5;
                    if (y_coarse == 29) {
                        y_coarse = 0;
                        ppu->v ^= 0x0800;
                    } else if (y_coarse == 31) {
                        y_coarse = 0;
                    } else {
                        y_coarse += 1;
                    }
                    ppu->v = (ppu->v & ~0x03E0) | (y_coarse << 5);
                }
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 2)) && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                ppu->v &= ~0x041F;
                ppu->v |= ppu->t & 0x041F;
            } else if (ppu->cycle == SCANLINE_IRQ_CYCLE && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                PPUBusScanlineIRQ(ppu->ppu_bus);
            } else if (ppu->cycle == SCANLINE_LAST_CYCLE) {
                // TODO: reset oam
                ppu->scanline++;
                ppu->cycle = 0;
            }

            if (ppu->scanline == RENDER_SCANLINE_END) {
                ppu->render_state = POST_RENDER;
            }
            break;
        case POST_RENDER: 
            if (ppu->cycle == SCANLINE_LAST_CYCLE) {
                ppu->scanline++;
                ppu->cycle = 0;
            }

            if (ppu->scanline == NTSC_POST_RENDER_SCANLINE_END) {
                ppu->render_state = VERTICAL_BLANKING;
            }
            break;
        case VERTICAL_BLANKING: 
            if (ppu->cycle == 1 && ppu->scanline == NTSC_POST_RENDER_SCANLINE_END) {
                ppu->status_register |= VERTICAL_BLANK_BIT;
                if (ppu->ctrl_register & GENERATE_NMI_BIT) {
                    interrupt_cpu = true;
                }
            } else if (ppu->cycle == SCANLINE_LAST_CYCLE) {
                ppu->scanline++;
                ppu->cycle = 0;
            }

            if (ppu->scanline == NTSC_VERTICAL_BLANKING_SCANLINE_END) {
                ppu->render_state = FINISHED;
            }
            break;
        case PRE_RENDER: 
            if (ppu->cycle == 1) {
                ppu->status_register &= ~(SPRITE_ZERO_HIT_BIT | VERTICAL_BLANK_BIT);
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 2)) && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                ppu->v &= ~0x041F;
                ppu->v |= ppu->t & 0x041F;
            } else if (ppu->cycle >= 281 && ppu->cycle <= 305) {    // hard to get information on this but i think this should last 25 cycles and it starts at 281 because cycles and dots are offset by 1
                ppu->v &= ~0x7BE0;
                ppu->v |= ppu->t & 0x7BE0;
            } else if (ppu->cycle == SCANLINE_IRQ_CYCLE && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                PPUBusScanlineIRQ(ppu->ppu_bus);
            } else if (ppu->cycle == (SCANLINE_LAST_CYCLE - 1) && (ppu->is_odd_frame && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT)))) {
                ppu->scanline++;
                ppu->cycle = 0;
            } else if (ppu->cycle == SCANLINE_LAST_CYCLE) {
                ppu->scanline++;
                ppu->cycle = 0;
            }

            if (ppu->scanline == NTSC_PRE_RENDER_SCANLINE_END) {
                ppu->render_state = RENDER;
                ppu->is_odd_frame = !(ppu->is_odd_frame);
            }
            break;
        case FINISHED: 
            printf("ppu shouldn't be clocked when it's state is set to FINISHED\n");
            exit(1);
            break;
    }
    ppu->cycle++;

    return interrupt_cpu;
}

bool PPUClockPAL(struct PPU* ppu) {
    printf("PAL not implemented\n");
    exit(1);
}


void PPUWriteCtrl(struct PPU* ppu, const uint8_t data) {
    ppu->ctrl_register = data;

    ppu->t &= ~((uint16_t)BASE_NAMETABLE_BITS << 10);
    ppu->t |= (ppu->ctrl_register & BASE_NAMETABLE_BITS) << 10;
}

void PPUWriteMask(struct PPU* ppu, const uint8_t data) {
    ppu->mask_register = data;
}

void PPUWriteOAMAddress(struct PPU* ppu, const uint8_t data) {
    ppu->oam_address_register = data;
}

void PPUWriteOAMData(struct PPU* ppu, const uint8_t data) {
    ppu->OAM[ppu->oam_address_register] = data;
    ppu->oam_address_register++;
}

void PPUWriteScroll(struct PPU* ppu, const uint8_t data) {
    if (ppu->w) {
        // 2. write
        ppu->t &= 0b1000110000011111;
        //ppu->t &= ~0x73E0;
        ppu->t |= ((data & 0b00000111) << 12) | ((data & 0x11111000) << 2);
        ppu->w = 0; 
    } else {
        // 1. write
        ppu->t &= 0b1111111111100000;
        ppu->t |= (data >> 3);// this is unneceseary: & 0b0000000000011111;
        ppu->x = data & 0x0007;
        ppu->w = 1;
    }
}

void PPUWritePPUAddress(struct PPU* ppu, const uint8_t data) {
    if (ppu->w) {
        // 2. write
        ppu->t &= 0xFF00;
        ppu->t |= data;
        ppu->v = ppu->t;
        ppu->w = 0; 
    } else {
        // 1. write
        ppu->t &= 0x00FF;
        ppu->t |= ((data & 0b00111111) << 8);
        ppu->w = 1;
    }
}

void PPUWritePPUData(struct PPU* ppu, const uint8_t data) {
    PPUBusWrite(ppu->ppu_bus, ppu->v, data);
    ppu->v += ((ppu->ctrl_register & VRAM_ADDRESS_INCREMENT_BIT) ? 32 : 1);
}

void PPUWriteDMAAddress(struct PPU* ppu, const uint8_t data) {
    printf("dma not implemented\n");
    exit(1);
}


uint8_t PPUReadStatus(struct PPU* ppu) {
    uint8_t temp = ppu->status_register;
    ppu->w = 0;
    ppu->status_register &= ~VERTICAL_BLANK_BIT;
    return temp;
}

uint8_t PPUReadOAMData(struct PPU* ppu) {
    return ppu->OAM[ppu->oam_address_register];
}

uint8_t PPUReadPPUData(struct PPU* ppu) {
    uint8_t temp = ppu->ppu_data_buffer;
    ppu->ppu_data_buffer = PPUBusRead(ppu->ppu_bus, ppu->v);
    if (ppu->v >= 0x3F00) {
        temp = ppu->ppu_data_buffer;
    }
    ppu->v += ((ppu->ctrl_register & VRAM_ADDRESS_INCREMENT_BIT) ? 32 : 1);
    return temp;
}

