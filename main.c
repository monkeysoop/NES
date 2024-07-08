#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


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


    SDL_Window* window = SDL_CreateWindow(
        "Basic C SDL project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * 2, 
        SCREEN_HEIGHT * 2,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Window creation] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
    }

    SDL_Rect squareRect;
    squareRect.w = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
    squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;


    uint32_t* pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));


    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );


    Uint64 previous_time = SDL_GetTicks();
	int frame_counter = 0;

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
					if (ev.key.keysym.sym == SDLK_ESCAPE || ev.key.keysym.sym == SDLK_q) {
						quit = true;
                    }
					//app.KeyboardDown(ev.key);
					break;
				case SDL_KEYUP:
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

        //SDL_LockTexture(texture, NULL, NULL, NULL);
		render(pixels, previous_time);
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
        //SDL_UnlockTexture(texture);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        int w;
        int h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        SDL_Rect screen_rect = {.x=0, .y=0, .w=w, .h=h};
        
        SDL_RenderCopyEx(
            renderer, 
            texture, 
            NULL, 
            &screen_rect,
            0.0,
            NULL,
            SDL_FLIP_NONE
        );
        
        SDL_RenderPresent(renderer);
        

		frame_counter++;
        Uint64 current_time = SDL_GetTicks64();
		if ((current_time - previous_time) > 1000) {
            printf("FPS: %d\t\tframe time: %.2f ms\n", frame_counter, (1000.0 / (float)frame_counter));
			frame_counter = 0;
			previous_time += 1000;
		}
	}

    SDL_DestroyTexture(texture);
    free(pixels);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
