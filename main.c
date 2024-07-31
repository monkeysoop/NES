#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

#include "emulator.h"



#define FONT_TEXTURE_CHAR_SIZE 8
#define FONT_TEXTURE_CHARS_WIDTH 16
#define FONT_TEXTURE_CHARS_HEIGHT 16





#define MAX_2(a, b) (((a) > (b)) ? (a) : (b))
#define MIN_2(a, b) (((a) < (b)) ? (a) : (b))

# define MAX_3(a, b, c) MAX_2(a, MAX_2(b, c))

struct DebugLayout {
    uint8_t zero_page_offset_x;
    uint8_t zero_page_offset_y;

    uint8_t registers_offset_x;
    uint8_t registers_offset_y;

    uint8_t disassembly_offset_x;
    uint8_t disassembly_offset_y;

    uint8_t palette_offset_x;
    uint8_t palette_offset_y;

    uint8_t pattern_table_offset_x;
    uint8_t pattern_table_offset_y;

    uint8_t disassembly_active_row_y;
    uint8_t selected_palette;

    uint8_t width;
    uint8_t height;
};

struct MainWindow {
    SDL_Window* window; 
    SDL_Renderer* renderer; 
    SDL_Texture* texture;

    uint32_t pixels_buffer[NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT];
};

struct DebugWindow {
    SDL_Window* window; 
    SDL_Renderer* renderer; 
    SDL_Texture* texture;
    SDL_Texture* font_texture;

    struct DebugLayout layout;

    char disassembly_buffer[DISASSEMBLY_BUFFER_HEIGHT][DISASSEMBLY_BUFFER_WIDTH];
    char zero_page_buffer[ZERO_PAGE_BYTE_BUFFER_HEIGHT][ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH];
    char registers_buffer[REGISTERS_BUFFER_HEIGHT][REGISTER_WIDTH]; 

    uint8_t palette_buffer[PALETTE_BUFFER_HEIGHT][PALETTE_BUFFER_WIDTH];

    uint32_t pattern_tables_pixels_buffer[2][PATTERN_TABLE_WIDTH * PATTERN_TABLE_HEIGHT];
};


void Clean(struct MainWindow main_window, struct DebugWindow debug_window) {

    if (debug_window.font_texture != NULL) {
        SDL_DestroyTexture(debug_window.font_texture);
    }
    if (debug_window.texture != NULL) {
        SDL_DestroyTexture(debug_window.texture);
    }
    if (debug_window.renderer != NULL) {
        SDL_DestroyRenderer(debug_window.renderer);
    }
    if (debug_window.window != NULL) {
        SDL_DestroyWindow(debug_window.window);
    }
    if (main_window.texture != NULL) {
        SDL_DestroyTexture(main_window.texture);
    }
    if (main_window.renderer != NULL) {
        SDL_DestroyRenderer(main_window.renderer);
    }
    if (main_window.window != NULL) {
        SDL_DestroyWindow(main_window.window);
    }
}

void Init(struct MainWindow* main_window, struct DebugWindow* debug_window) {

    main_window->window = SDL_CreateWindow(
        "NES emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600, 
        400,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (main_window->window == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Window creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }


    main_window->renderer = SDL_CreateRenderer(main_window->window, -1, SDL_RENDERER_ACCELERATED);
    if (main_window->renderer == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    main_window->texture = SDL_CreateTexture(
        main_window->renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        NES_SCREEN_WIDTH,
        NES_SCREEN_HEIGHT
    );
    if (main_window->texture == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Texture creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }




    debug_window->window = SDL_CreateWindow(
        "NES debug view",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        debug_window->layout.width * FONT_TEXTURE_CHAR_SIZE, 
        debug_window->layout.height * FONT_TEXTURE_CHAR_SIZE,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (debug_window->window == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Debug window creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    debug_window->renderer = SDL_CreateRenderer(debug_window->window, -1, SDL_RENDERER_ACCELERATED);
    if (debug_window->renderer == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    

    debug_window->texture = SDL_CreateTexture(
        debug_window->renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        debug_window->layout.width * FONT_TEXTURE_CHAR_SIZE,
        debug_window->layout.height * FONT_TEXTURE_CHAR_SIZE
    );
    if (debug_window->texture == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Texture loading] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }

    
    debug_window->font_texture = IMG_LoadTexture(debug_window->renderer, "font.png");
    if (debug_window->font_texture == NULL) {
        Clean(*main_window, *debug_window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Texture loading] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
}


void MainRender(struct MainWindow main_window) {
    SDL_UpdateTexture(main_window.texture, NULL, main_window.pixels_buffer, NES_SCREEN_WIDTH * sizeof(uint32_t));
        
    SDL_SetRenderDrawColor(main_window.renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(main_window.renderer);
    
    
    SDL_RenderCopyEx(
        main_window.renderer, 
        main_window.texture, 
        NULL, 
        NULL,
        0.0,
        NULL,
        SDL_FLIP_NONE
    );
    
    SDL_RenderPresent(main_window.renderer);
}


void DebugRender(struct DebugWindow debug_window) {
    SDL_SetRenderTarget(debug_window.renderer, debug_window.texture);

    // disassembly
    SDL_SetTextureColorMod(debug_window.font_texture, 0xFF, 0x00, 0x00);
    for (int y = 0; y < DISASSEMBLY_BUFFER_HEIGHT; y++) {
        if (y == debug_window.layout.disassembly_active_row_y) {
            SDL_SetTextureColorMod(debug_window.font_texture, 0x00, 0x00, 0xFF);
        }
        for (int x = 0; x < DISASSEMBLY_BUFFER_WIDTH; x++) {
            uint8_t c = (uint8_t)debug_window.disassembly_buffer[y][x];
    
            uint8_t c_x = c % FONT_TEXTURE_CHARS_WIDTH;
            uint8_t c_y = c / FONT_TEXTURE_CHARS_WIDTH;
    
            SDL_Rect char_rect = {
                .x=(c_x * FONT_TEXTURE_CHAR_SIZE), 
                .y=(c_y * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE};

            SDL_Rect target_rect = {
                .x =((debug_window.layout.disassembly_offset_x + x) * FONT_TEXTURE_CHAR_SIZE), 
                .y=((debug_window.layout.disassembly_offset_y + y) * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE
            };

            SDL_RenderCopyEx(
                debug_window.renderer, 
                debug_window.font_texture, 
                &char_rect,
                &target_rect,
                0.0,
                NULL,
                SDL_FLIP_NONE
            );
        }
        if (y == debug_window.layout.disassembly_active_row_y) {
            SDL_SetTextureColorMod(debug_window.font_texture, 0xFF, 0x00, 0x00);
        }
    }

    // zero page
    SDL_SetTextureColorMod(debug_window.font_texture, 0x00, 0xFF, 0x00);
    for (int y = 0; y < ZERO_PAGE_BYTE_BUFFER_HEIGHT; y++) {
        for (int x = 0; x < ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH; x++) {
            uint8_t c = (uint8_t)debug_window.zero_page_buffer[y][x];
    
            uint8_t c_x = c % FONT_TEXTURE_CHARS_WIDTH;
            uint8_t c_y = c / FONT_TEXTURE_CHARS_WIDTH;
    
            SDL_Rect char_rect = {
                .x=(c_x * FONT_TEXTURE_CHAR_SIZE), 
                .y=(c_y * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE};

            SDL_Rect target_rect = {
                .x =((debug_window.layout.zero_page_offset_x + x) * FONT_TEXTURE_CHAR_SIZE), 
                .y=((debug_window.layout.zero_page_offset_y + y) * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE
            };

            SDL_RenderCopyEx(
                debug_window.renderer, 
                debug_window.font_texture, 
                &char_rect,
                &target_rect,
                0.0,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }

    // registers
    SDL_SetTextureColorMod(debug_window.font_texture, 0xFF, 0x00, 0xFF);
    for (int y = 0; y < REGISTERS_BUFFER_HEIGHT; y++) {
        for (int x = 0; x < REGISTER_WIDTH; x++) {
            uint8_t c = (uint8_t)debug_window.registers_buffer[y][x];
    
            uint8_t c_x = c % FONT_TEXTURE_CHARS_WIDTH;
            uint8_t c_y = c / FONT_TEXTURE_CHARS_WIDTH;
    
            SDL_Rect char_rect = {
                .x=(c_x * FONT_TEXTURE_CHAR_SIZE), 
                .y=(c_y * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE};

            SDL_Rect target_rect = {
                .x =((debug_window.layout.registers_offset_x + x) * FONT_TEXTURE_CHAR_SIZE), 
                .y=((debug_window.layout.registers_offset_y + y) * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE
            };

            SDL_RenderCopyEx(
                debug_window.renderer, 
                debug_window.font_texture, 
                &char_rect,
                &target_rect,
                0.0,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }

    // palette
    for (int y = 0; y < PALETTE_BUFFER_HEIGHT; y++) {
        for (int x = 0; x < PALETTE_BUFFER_WIDTH; x++) {
            uint32_t color_rgba = nes_palette_colors_rgba[debug_window.palette_buffer[y][x]];

            SDL_SetRenderDrawColor(debug_window.renderer, ((color_rgba & 0xFF000000) >> 24), ((color_rgba & 0x00FF0000) >> 16) ,((color_rgba & 0x0000FF00) >> 8), (color_rgba & 0x000000FF));

            SDL_Rect target_rect = {
                .x =((debug_window.layout.palette_offset_x + x) * FONT_TEXTURE_CHAR_SIZE), 
                .y=((debug_window.layout.palette_offset_y + y) * FONT_TEXTURE_CHAR_SIZE), 
                .w=FONT_TEXTURE_CHAR_SIZE, 
                .h=FONT_TEXTURE_CHAR_SIZE
            };

            SDL_RenderFillRect(debug_window.renderer, &target_rect);
        }
    }
    
    // pattern table
    SDL_Rect pattern_table_target_rect_1 = {
        .x =((debug_window.layout.pattern_table_offset_x) * FONT_TEXTURE_CHAR_SIZE), 
        .y=((debug_window.layout.pattern_table_offset_y) * FONT_TEXTURE_CHAR_SIZE), 
        .w=PATTERN_TABLE_WIDTH, 
        .h=PATTERN_TABLE_HEIGHT
    };

    SDL_Rect pattern_table_target_rect_2 = {
        .x =((debug_window.layout.pattern_table_offset_x) * FONT_TEXTURE_CHAR_SIZE + PATTERN_TABLE_WIDTH),  // the offsets are in terms of characters but the PATTERN_TABLE_WIDTH is in pixels
        .y=((debug_window.layout.pattern_table_offset_y) * FONT_TEXTURE_CHAR_SIZE), 
        .w=PATTERN_TABLE_WIDTH, 
        .h=PATTERN_TABLE_HEIGHT
    };



    SDL_UpdateTexture(debug_window.texture, &pattern_table_target_rect_1, &(debug_window.pattern_tables_pixels_buffer[0][0]), PATTERN_TABLE_WIDTH * sizeof(uint32_t));
    SDL_UpdateTexture(debug_window.texture, &pattern_table_target_rect_2, debug_window.pattern_tables_pixels_buffer[1], PATTERN_TABLE_WIDTH * sizeof(uint32_t));



    SDL_SetRenderTarget(debug_window.renderer, NULL);
    
    
    SDL_RenderCopyEx(
        debug_window.renderer, 
        debug_window.texture, 
        NULL, 
        NULL,
        0.0,
        NULL,
        SDL_FLIP_NONE
    );

    SDL_RenderPresent(debug_window.renderer);
}



int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("please pass the name of the rom file (xxx.nes) as first parameter\n");
        exit(1);
    }


    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR);
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL initialization] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}
    IMG_Init(IMG_INIT_PNG);


    struct MainWindow main_window = {
        .window = NULL,
        .renderer = NULL,
        .texture = NULL,

        .pixels_buffer = { 0 },
    };


    struct DebugWindow debug_window = {
        .window = NULL,
        .renderer = NULL,
        .texture = NULL,
        .font_texture = NULL,

        .layout = {
            .zero_page_offset_x = 0,
            .zero_page_offset_y = 0,

            .registers_offset_x = MAX_3(ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH, PALETTE_BUFFER_WIDTH, ((2 * PATTERN_TABLE_WIDTH) / FONT_TEXTURE_CHAR_SIZE)),
            .registers_offset_y = 0,
            
            .disassembly_offset_x = MAX_3(ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH, PALETTE_BUFFER_WIDTH, ((2 * PATTERN_TABLE_WIDTH) / FONT_TEXTURE_CHAR_SIZE)),
            .disassembly_offset_y = REGISTERS_BUFFER_HEIGHT,

            .palette_offset_x = 0,
            .palette_offset_y = ZERO_PAGE_BYTE_BUFFER_HEIGHT,

            .pattern_table_offset_x = 0,
            .pattern_table_offset_y = ZERO_PAGE_BYTE_BUFFER_HEIGHT + PALETTE_BUFFER_HEIGHT,

            .disassembly_active_row_y = 0,
            .selected_palette = 0,

            .width = MAX_3(ZERO_PAGE_BYTE_BUFFER_WIDTH * ZERO_PAGE_BYTE_WIDTH, PALETTE_BUFFER_WIDTH, ((2 * PATTERN_TABLE_WIDTH) / FONT_TEXTURE_CHAR_SIZE)) + MAX_2(DISASSEMBLY_BUFFER_WIDTH, REGISTER_WIDTH),
            .height = MAX_2((REGISTERS_BUFFER_HEIGHT + DISASSEMBLY_BUFFER_HEIGHT), (ZERO_PAGE_BYTE_BUFFER_HEIGHT + PALETTE_BUFFER_HEIGHT + (PATTERN_TABLE_HEIGHT / FONT_TEXTURE_CHAR_SIZE))),
        },

        .disassembly_buffer = { { 0 } },
        .zero_page_buffer = { { 0 } },
        .registers_buffer = { { 0 } },
        
        .palette_buffer = { { 0 } },

        .pattern_tables_pixels_buffer = { { 0 } },
    };
    
    Init(&main_window, &debug_window);
    
    struct Emulator emulator;
    EmulatorInit(&emulator, argv[1]);
    


    SDL_HideWindow(debug_window.window);
    Uint64 previous_time = SDL_GetTicks();
	int frame_counter = 0;

    bool debug_shown = false;
    bool i_pressed = false;

    bool quit = false;
    while (!quit) {
		// amíg van feldolgozandó üzenet dolgozzuk fel mindet:
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_ESCAPE: quit = true; break;
                        case SDLK_q: quit = true; break;
                        case SDLK_SPACE: EmulatorRender(&emulator, main_window.pixels_buffer); break;
                        case SDLK_f: for (int i = 0; i < 100; i++) {EmulatorRender(&emulator, main_window.pixels_buffer); } break;
                        default: break;
                    }
					//app.KeyboardDown(ev.key);
					break;
				case SDL_KEYUP:
					switch (ev.key.keysym.sym) {
                        case SDLK_i: i_pressed = true; break;
                        case SDLK_r: EmulatorReset(&emulator); break;
                        case SDLK_t: emulator.cpu.registers.program_counter = 0xC000; break;
                        default: break;
                    }
                    //app.KeyboardUp(ev.key);
					break;
				case SDL_MOUSEBUTTONDOWN:
					//app.MouseDown(ev.button);
					break;
				case SDL_MOUSEBUTTONUP:
                    //app.MouseUp(ev.button);
					break;
				case SDL_MOUSEWHEEL:
					//app.MouseWheel(ev.wheel);
					break;
				case SDL_MOUSEMOTION:
					//app.MouseMove(ev.motion);
					break;
				case SDL_WINDOWEVENT:
                    if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
                        quit = true;
                    }
					break;
					//// Néhány platformon (pl. Windows) a SIZE_CHANGED nem hívódik meg az első megjelenéskor.
					//// Szerintünk ez bug az SDL könytárban.
					//// Ezért ezt az esetet külön lekezeljük, 
					//// mivel a MyApp esetlegesen tartalmazhat ablak méret függő beállításokat, pl. a kamera aspect ratioját a perspective() hívásnál.
					//if ((ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) || (ev.window.event == SDL_WINDOWEVENT_SHOWN)) {
					//	int w;
                    //    int h;
					//	SDL_GetRendererOutputSize(renderer, &w, &h);
					//	//app.Resize( w, h );
					//}
					//break;
			}
		}
    
        MainRender(main_window);
        
    
        if (i_pressed) {
            if (debug_shown) {
                SDL_HideWindow(debug_window.window);
            } else {
                SDL_ShowWindow(debug_window.window);
                SDL_RaiseWindow(debug_window.window);
            }
            debug_shown = !(debug_shown);
            i_pressed = false;
        }
    
        if (debug_shown) {
            uint16_t pc = emulator.cpu.registers.program_counter;
            uint16_t start_address = (pc >= (DISASSEMBLY_BUFFER_HEIGHT / 2)) ? (pc - (DISASSEMBLY_BUFFER_HEIGHT / 2)) : pc;
            
            debug_window.layout.disassembly_active_row_y = CPUDisassemble(
                &(emulator.cpu), 
                start_address, 
                DISASSEMBLY_BUFFER_HEIGHT, 
                debug_window.disassembly_buffer, 
                debug_window.zero_page_buffer, 
                debug_window.registers_buffer
            );
        
            DebugRender(debug_window);
        }
    
    
    
    
    	frame_counter++;
        Uint64 current_time = SDL_GetTicks64();
		if ((current_time - previous_time) > 1000) {
            printf("FPS: %d\t\tframe time: %.2f ms\n", frame_counter, (1000.0 / (float)frame_counter));
			frame_counter = 0;
			previous_time += 1000;
		}
	}

    EmulatorClean(&emulator);
    Clean(main_window, debug_window);
    SDL_Quit();
    return 0;
}
