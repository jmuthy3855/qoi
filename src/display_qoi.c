#include <stdio.h>
#include "display_qoi.h"
#include "SDL.h"
#include "SDL_image.h"
 
void print_qoi(qoi_app_struct *app) {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(app->header.width, app->header.height, 0, &window, &renderer);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // default color for testing

    for (int r = 0; r < app->header.height; r++) {
        for (int c = 0; c < app->header.width; c++) {
            uint8_t red = app->decoded_pixels[r][c].red;
            uint8_t green = app->decoded_pixels[r][c].green;
            uint8_t blue = app->decoded_pixels[r][c].blue;
            uint8_t alpha = app->decoded_pixels[r][c].alpha;

            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
            SDL_RenderDrawPoint(renderer, c, r); // x is col, y is row
        }
    }

    SDL_RenderPresent(renderer);
    
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
