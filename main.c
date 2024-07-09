#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))



void clean(SDL_Window* main_window, SDL_Renderer* main_renderer, SDL_Texture* main_texture, SDL_Window* debug_window, SDL_Renderer* debug_renderer, SDL_Texture* debug_texture) {
    if (debug_window != NULL) {
        SDL_DestroyTexture(debug_texture);
    }
    if (debug_renderer != NULL) {
        SDL_DestroyRenderer(debug_renderer);
    }
    if (debug_texture != NULL) {
        SDL_DestroyWindow(debug_window);
    }
    if (main_texture != NULL) {
        SDL_DestroyTexture(main_texture);
    }
    if (main_renderer != NULL) {
        SDL_DestroyRenderer(main_renderer);
    }
    if (main_window != NULL) {
        SDL_DestroyWindow(main_window);
    }
}

void init(SDL_Window** main_window, SDL_Renderer** main_renderer, SDL_Texture** main_texture, SDL_Window** debug_window, SDL_Renderer** debug_renderer, SDL_Texture** debug_texture) {
    (*main_window) = SDL_CreateWindow(
        "NES emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600, 
        400,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if ((*main_window) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Window creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }


    (*main_renderer) = SDL_CreateRenderer((*main_window), -1, SDL_RENDERER_ACCELERATED);
    if ((*main_renderer) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    (*main_texture) = SDL_CreateTexture(
        (*main_renderer), 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    if ((*main_texture) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Texture creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    (*debug_window) = SDL_CreateWindow(
        "NES debug view",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600, 
        400,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if ((*debug_window) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Debug window creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    (*debug_renderer) = SDL_CreateRenderer((*debug_window), -1, SDL_RENDERER_ACCELERATED);
    if ((*debug_renderer) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
    
    
    (*debug_texture) = IMG_LoadTexture((*debug_renderer), "font.png");
    if ((*debug_texture) == NULL) {
        clean((*main_window), (*main_renderer), (*main_texture), (*debug_window), (*debug_renderer), (*debug_texture));
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Texture loading] Error during the SDL initialization: %s", SDL_GetError());
		exit(1);
    }
}



void render(uint32_t* pixels, Uint64 time) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            pixels[y * SCREEN_WIDTH + x] = 0x000000FF | (((x / 8) + ((y / 8) % 2)) % 2) * 0x77000000; 
        }
    }
}


int main(int argc, char** argv)
{

    SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR);
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL initialization] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}
    IMG_Init(IMG_INIT_PNG);

    
    SDL_Window* main_window = NULL;
    SDL_Renderer* main_renderer = NULL;
    SDL_Texture* main_texture = NULL;
    SDL_Window* debug_window = NULL;
    SDL_Renderer* debug_renderer = NULL;
    SDL_Texture* debug_texture = NULL;

    init(&main_window, &main_renderer, &main_texture, &debug_window, &debug_renderer, &debug_texture);

    

    uint32_t* pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));



    SDL_HideWindow(debug_window);
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
                        default: break;
                    }
					//app.KeyboardDown(ev.key);
					break;
				case SDL_KEYUP:
					switch (ev.key.keysym.sym) {
                        case SDLK_i: i_pressed = true; break;
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

        if (i_pressed) {
            (debug_shown) ? SDL_HideWindow(debug_window) : SDL_ShowWindow(debug_window);
            debug_shown = !(debug_shown);
            i_pressed = false;
        }



		render(pixels, previous_time);
        SDL_UpdateTexture(main_texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
        
        SDL_SetRenderDrawColor(main_renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderClear(main_renderer);
        
        int w;
        int h;
        SDL_GetRendererOutputSize(main_renderer, &w, &h);
        
        SDL_Rect screen_rect = {.x=0, .y=0, .w=w, .h=h};
        
        SDL_RenderCopyEx(
            main_renderer, 
            main_texture, 
            NULL, 
            &screen_rect,
            0.0,
            NULL,
            SDL_FLIP_NONE
        );
        
        SDL_RenderPresent(main_renderer);
        

        if (debug_shown) {
            SDL_RenderCopy(debug_renderer, debug_texture, NULL, NULL);
            SDL_RenderPresent(debug_renderer);
        }




		frame_counter++;
        Uint64 current_time = SDL_GetTicks64();
		if ((current_time - previous_time) > 1000) {
            printf("FPS: %d\t\tframe time: %.2f ms\n", frame_counter, (1000.0 / (float)frame_counter));
			frame_counter = 0;
			previous_time += 1000;
		}
	}

    free(pixels);
    clean(main_window, main_renderer, main_texture, debug_window, debug_renderer, debug_texture);
    SDL_Quit();
    return 0;
}
