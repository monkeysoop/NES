#include <string.h>
#include <stdio.h>

#include "ppu.h"
#include "logger.h"


void PPUInit(struct PPU* ppu, struct PPUBus* ppu_bus, enum TVSystem tv_system) {
    ppu->ctrl_register = 0;
    ppu->mask_register = 0;
    ppu->status_register = 0;
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
    memset(ppu->scanline_OAM_indecies, 0, 8 * sizeof(uint8_t));
    ppu->scanline_OAM_length = 0;

    ppu->render_state = PRE_RENDER;
    ppu->scanline = (tv_system == NTSC) ? NTSC_VERTICAL_BLANKING_SCANLINE_END : PAL_VERTICAL_BLANKING_SCANLINE_END;
    ppu->cycle = 0;

    ppu->is_odd_frame = 0;

    ppu->sprite_0_hit_happened = false;

    ppu->ppu_bus = ppu_bus;
}

void PPUReset(struct PPU* ppu, enum TVSystem tv_system) {
    ppu->ctrl_register = 0;
    ppu->mask_register = 0;
    ppu->status_register = ppu->status_register & VERTICAL_BLANK_BIT;
    ppu->scroll_register = 0;
    ppu->ppu_data_register = 0;

    ppu->v = 0;
    ppu->t = 0;
    ppu->t = 0;
    ppu->w = 0;

    ppu->ppu_data_buffer = 0;

    memset(ppu->OAM, 0, 256 * sizeof(uint8_t));
    memset(ppu->scanline_OAM_indecies, 0, 8 * sizeof(uint8_t));
    ppu->scanline_OAM_length = 0;
    
    ppu->render_state = PRE_RENDER;
    ppu->scanline = (tv_system == NTSC) ? NTSC_VERTICAL_BLANKING_SCANLINE_END : PAL_VERTICAL_BLANKING_SCANLINE_END;
    ppu->cycle = 0;

    ppu->is_odd_frame = 0;
 
    ppu->sprite_0_hit_happened = false;
}


enum GenerateInterrupt PPUClockNTSC(struct PPU* ppu, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]) {
    // the reason why the cartridge gets interrupted from here and the cpu isn't is 
    // because PPU struct can't have a reference to CPU (that would create circular dependency)
    // so either is has a void pointer to a callback or informs the emulator trough the return value, which interrupts it
    enum GenerateInterrupt generate_interrupt = GENERATE_NO_INTERRUPT;

    switch (ppu->render_state) {
        case RENDER: 
            if (ppu->cycle > 0 && ppu->cycle <= SCANLINE_VISIBLE_DOTS) {
                uint8_t dot = ppu->cycle - 1;   // dot is basically x
                uint8_t fine_x = (ppu->x + dot) & 0x07;
                
                uint8_t background_color_address = 0;
                bool background_opaque = false;

                if (ppu->mask_register & SHOW_BACKGROUND_BIT) {
                    if ((ppu->mask_register & SHOW_BACKGROUND_LEFTMOST_BIT) || dot >= 8) {
                        // render background
                        uint16_t tile_address = 0x2000 | (ppu->v & 0x0FFF);

                        uint16_t pattern_address = (((uint16_t)PPUBusRead(ppu->ppu_bus, tile_address) << 4) + ((ppu->v >> 12) & 0x0007)) 
                                                 | ((ppu->ctrl_register & BACKGROUND_PATTERN_TABLE_ADDRESS_BIT) << 8);

                        background_color_address = ((PPUBusRead(ppu->ppu_bus, pattern_address) >> (fine_x ^ 0x07)) & 0x01)
                                                 | (((PPUBusRead(ppu->ppu_bus, (pattern_address + 8)) >> (fine_x ^ 0x07)) & 0x01) << 1);
                        
                        background_opaque = (bool)background_color_address;

                        uint16_t attribute_address = 0x23C0 | (ppu->v & 0x0C00) | ((ppu->v >> 4) & 0x0038) | ((ppu->v >> 2) & 0x0007);
                        background_color_address |= (((PPUBusRead(ppu->ppu_bus, attribute_address) >> (((ppu->v >> 4) & 0x04) | (ppu->v & 0x02))) & 0x03) << 2);
                    }

                    if (fine_x == 7) {
                        if ((ppu->v & 0x001F) == 0x001F) {
                            ppu->v &= ~0x001F;
                            ppu->v ^= 0x0400;
                        } else {
                            ppu->v++;
                        }
                    }
                }


                uint8_t sprite_color_address = 0;
                bool sprite_opaque = false;

                bool sprite_in_foreground = false;

                if ((ppu->mask_register & SHOW_SPRITES_BIT) && ((ppu->mask_register & SHOW_SPRITES_LEFTMOST_BIT) || dot >= 8)) {
                    uint8_t height = (ppu->ctrl_register & SPRITE_SIZE_BIT) ? 16 : 8;
                    int i = 0;
                    while (!sprite_opaque && i < ppu->scanline_OAM_length) {
                        uint8_t sprite_y =          ppu->OAM[ppu->scanline_OAM_indecies[i]    ] + 1;
                        uint8_t sprite_index =      ppu->OAM[ppu->scanline_OAM_indecies[i] + 1];
                        uint8_t sprite_attributes = ppu->OAM[ppu->scanline_OAM_indecies[i] + 2];
                        uint8_t sprite_x =          ppu->OAM[ppu->scanline_OAM_indecies[i] + 3];

                        
                        if ((int)(dot - sprite_x) >= 0 && (int)(dot - sprite_x) < 8) {
                            // note subtracting 2 unsigned numbers in both x and y case will result in positive numbers because of prior checks assuring it 
                            uint8_t shift_x = (dot - sprite_x) % 8;
                            uint8_t shift_y = (ppu->scanline - sprite_y) % height;

                            if (!(sprite_attributes & FLIP_SPRITE_HORIZONTALLY_BIT)) {
                                shift_x ^= 0x07;
                            }
                            if (sprite_attributes & FLIP_SPRITE_VERTICALLY_BIT) {
                                shift_y ^= (height - 1);
                            }

                            uint16_t pattern_address;
                            if (ppu->ctrl_register & SPRITE_SIZE_BIT) {
                                // 16 pixel tall tiles
                                pattern_address = ((uint16_t)(sprite_index & 0b11111110) << 4) + ((shift_y & 0x07) | ((shift_y & 0x08) << 1));
                                pattern_address |= (sprite_index & 0b00000001) ? 0x1000 : 0;
                            } else {
                                // 8 pixel tall tiles
                                pattern_address = ((uint16_t)sprite_index << 4) + shift_y + ((ppu->ctrl_register & SPRITE_PATTERN_TABLE_ADDRESS_BIT) ? 0x1000 : 0);
                            }

                            sprite_color_address = ((PPUBusRead(ppu->ppu_bus, pattern_address) >> shift_x) & 0x01)
                                                 | (((PPUBusRead(ppu->ppu_bus, (pattern_address + 8)) >> shift_x) & 0x01) << 1);

                            sprite_opaque = (bool)sprite_color_address;

                            sprite_color_address |= 0x10;
                            sprite_color_address |= (sprite_attributes & SPRITE_PALETTE_BITS) << 2;
                        
                            sprite_in_foreground = !((bool)(sprite_attributes & SPRITE_PRIORITY_BIT));
                        }

                        i++;
                    };


                    if (sprite_opaque && background_opaque && ppu->scanline_OAM_indecies[i - 1] == 0 && dot != 255 && !ppu->sprite_0_hit_happened) {
                        ppu->status_register |= SPRITE_ZERO_HIT_BIT;
                        ppu->sprite_0_hit_happened = true;
                    }
                }

                uint8_t color_address = background_color_address;
                if (!background_opaque && !sprite_opaque) {
                    color_address = 0;
                    if ((ppu->v & 0x3F00) == 0x3F00 && !(ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                        color_address = ppu->v & 0x1F;
                    }
                } else if (sprite_opaque && (!background_opaque || sprite_in_foreground)) {
                    color_address = sprite_color_address;
                }
                uint32_t color_rgba = nes_palette_colors_rgba[PPUBusRead(ppu->ppu_bus, (0x3F00 + color_address))];
                pixels_buffer[ppu->scanline * NES_SCREEN_WIDTH + dot] = color_rgba;
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 1)) && (ppu->mask_register & SHOW_BACKGROUND_BIT)) {
                if ((ppu->v & 0x7000) != 0x7000) {
                    ppu->v += 0x1000;
                } else {
                    ppu->v &= ~0x7000;
                    uint16_t coarse_y = (ppu->v & 0x03E0) >> 5;
                    if (coarse_y == 29) {
                        coarse_y = 0;
                        ppu->v ^= 0x0800;
                    } else if (coarse_y == 31) {
                        coarse_y = 0;
                    } else {
                        coarse_y++;
                    }
                    ppu->v = (ppu->v & ~0x03E0) | (coarse_y << 5);
                }
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 2)) && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                ppu->v &= ~0x041F;
                ppu->v |= ppu->t & 0x041F;
            } else if (ppu->cycle == SCANLINE_IRQ_CYCLE && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                if (PPUBusScanlineIRQ(ppu->ppu_bus)) {
                    generate_interrupt = GENERATE_IRQ;
                }
            } else if (ppu->cycle == SCANLINE_LAST_CYCLE) { // checking if sprite rendering is enabled is unnecesseary because if it's disabled than the oam cache won't be read                
                ppu->scanline++;
                ppu->cycle = 0;


                uint8_t height = (ppu->ctrl_register & SPRITE_SIZE_BIT) ? 16 : 8;
                ppu->scanline_OAM_length = 0;

                for (int oam_index = ppu->oam_address_register / 4; oam_index < 64; oam_index++) {
                    uint8_t sprite_y = ppu->OAM[oam_index * 4];
                    if (sprite_y <= (ppu->scanline - 1) && (ppu->scanline - 1) < (sprite_y + height)) {     // note that scanline has already been incremented but the way sprite y is implemented it needs to be offset one less
                        if (ppu->scanline_OAM_length < SCANLINE_OAM_BUFFER_SIZE) {
                            ppu->scanline_OAM_indecies[ppu->scanline_OAM_length] = oam_index * 4;
                            ppu->scanline_OAM_length++;
                        } else {
                            if (ppu->scanline < RENDER_SCANLINE_END && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                                ppu->status_register |= SPRITE_OVERFLOW_BIT;
                            }
                            break;
                        }
                    }
                }
            }

            if (ppu->scanline == RENDER_SCANLINE_END) {
                ppu->scanline_OAM_length = 0;
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
                    generate_interrupt = GENERATE_NMI;
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
                ppu->status_register &= ~(SPRITE_ZERO_HIT_BIT | VERTICAL_BLANK_BIT | SPRITE_OVERFLOW_BIT);
                ppu->sprite_0_hit_happened = false;
            } else if ((ppu->cycle == (SCANLINE_VISIBLE_DOTS + 2)) && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                ppu->v &= ~0x041F;
                ppu->v |= ppu->t & 0x041F;
            } else if (ppu->cycle >= 281 && ppu->cycle <= 305 && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {    // hard to get information on this but i think this should last 25 cycles and it starts at 281 because cycles and dots are offset by 1
                ppu->v &= ~0x7BE0;
                ppu->v |= ppu->t & 0x7BE0;
            } else if (ppu->cycle == SCANLINE_IRQ_CYCLE && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                if (PPUBusScanlineIRQ(ppu->ppu_bus)) {
                    generate_interrupt = GENERATE_IRQ;
                }
            } else if (ppu->cycle == (SCANLINE_LAST_CYCLE - 1) && ppu->is_odd_frame && (ppu->mask_register & (SHOW_BACKGROUND_BIT | SHOW_SPRITES_BIT))) {
                ppu->scanline++;
                ppu->cycle = 0;
            } else if (ppu->cycle == SCANLINE_LAST_CYCLE) {
                ppu->scanline++;
                ppu->cycle = 0;
            }

            if (ppu->scanline == NTSC_PRE_RENDER_SCANLINE_END) {
                ppu->render_state = RENDER;
                ppu->scanline = 0;
                ppu->is_odd_frame = !(ppu->is_odd_frame);
            }
            break;
        case FINISHED: 
            LOG(ERROR, PPU, "ppu shouldn't be clocked when it's state is set to FINISHED\n");
            break;
    }
    ppu->cycle++;

    return generate_interrupt;
}

enum GenerateInterrupt PPUClockPAL(struct PPU* ppu, uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT]) {
    LOG(ERROR, PPU, "PAL not implemented\n");
}


void DebugView(
    struct PPU* ppu, 
    uint8_t palette_buffer[PALETTE_BUFFER_HEIGHT][PALETTE_BUFFER_WIDTH], 
    uint32_t pattern_tables_pixels_buffer[2][PATTERN_TABLE_WIDTH * PATTERN_TABLE_HEIGHT],
    uint8_t selected_palette,
    char nametable_buffer[NAMETABLE_BYTE_BUFFER_HEIGHT][NAMETABLE_BYTE_BUFFER_WIDTH * NAMETABLE_BYTE_WIDTH],
    uint8_t selected_nametable
) {
    for (int y = 0; y < PALETTE_BUFFER_HEIGHT; y++) {
        for (int x = 0; x < PALETTE_BUFFER_WIDTH; x++) {
            palette_buffer[y][x] = PPUBusRead(ppu->ppu_bus, 0x3F00 + (y * PALETTE_BUFFER_WIDTH + x));
        }
    }


    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t tile_y = 0; tile_y < (PATTERN_TABLE_HEIGHT / 8); tile_y++) {
            for (uint8_t tile_x = 0; tile_x < (PATTERN_TABLE_WIDTH / 8); tile_x++) {
                for (uint8_t fine_y = 0; fine_y < 8; fine_y++) {
                    uint8_t tile_id = tile_y * (PATTERN_TABLE_WIDTH / 8) + tile_x;
                    
                    uint16_t pattern_address_lower = (i * 0x1000) | (tile_id << 4) | 0x0000 | fine_y;
                    uint16_t pattern_address_upper = (i * 0x1000) | (tile_id << 4) | 0x0008 | fine_y;

                    uint8_t background_color_address_lower = PPUBusRead(ppu->ppu_bus, pattern_address_lower);
                    uint8_t background_color_address_upper = PPUBusRead(ppu->ppu_bus, pattern_address_upper);

                    for (uint8_t fine_x = 0; fine_x < 8; fine_x++) {
                        uint8_t background_color_address = ((background_color_address_lower >> (fine_x ^ 0x07)) & 0x01)
                                                         | ((background_color_address_upper >> ((fine_x ^ 0x07) - 1)) & 0x02);

                        pattern_tables_pixels_buffer[i][(tile_y * 8 + fine_y) * PATTERN_TABLE_WIDTH + (tile_x * 8 + fine_x)] = nes_palette_colors_rgba[PPUBusRead(ppu->ppu_bus, 0x3F00 | ((selected_palette & 0x07) << 2) | background_color_address)];
                    }
                }
            }
        }
    }


    uint16_t address_offset = 0;
    switch (selected_nametable) {
        case 0: address_offset = 0x2000; break;
        case 1: address_offset = 0x2400; break;
        case 2: address_offset = 0x2800; break;
        case 3: address_offset = 0x2C00; break;
        default:
            LOG(ERROR, PPU, "select a nametable 0: 0x2000  1: 0x2400  2: 0x2800 3: 0x2C00\n");
    }

    char name_table_row_buffer[NAMETABLE_BYTE_BUFFER_WIDTH * NAMETABLE_BYTE_WIDTH + 1];

    for (int y = 0; y < NAMETABLE_BYTE_BUFFER_HEIGHT; y++) {
        for (int x = 0; x < NAMETABLE_BYTE_BUFFER_WIDTH; x++) {
            uint16_t temp_address = address_offset | (y * NAMETABLE_BYTE_BUFFER_WIDTH + x);
            snprintf(&name_table_row_buffer[x * NAMETABLE_BYTE_WIDTH], (NAMETABLE_BYTE_WIDTH + 1) * sizeof(char), "%02X ", PPUBusRead(ppu->ppu_bus, temp_address));
        }
        memcpy(&nametable_buffer[y], &name_table_row_buffer[0], NAMETABLE_BYTE_BUFFER_WIDTH * NAMETABLE_BYTE_WIDTH * sizeof(char));
    }
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
        ppu->t |= ((data & 0b00000111) << 12) | ((data & 0b11111000) << 2);
        ppu->w = 0; 
    } else {
        // 1. write
        ppu->t &= 0b1111111111100000;
        ppu->t |= ((data & 0b11111000) >> 3);
        ppu->x = data & 0b00000111;
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

