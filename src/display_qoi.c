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
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for (int r = 0; r < app->curr_row; r++) {
        int y_pixels = 0;

        if (r < app->curr_row - 1) {
            y_pixels = app->header.width;
        } else {
            y_pixels = app->curr_col;
        }

        for (int c = 0; c < y_pixels; c++) {
            uint8_t red = app->decoded_pixels[app->curr_row][app->curr_col].red;
            uint8_t green = app->decoded_pixels[app->curr_row][app->curr_col].green;
            uint8_t blue = app->decoded_pixels[app->curr_row][app->curr_col].blue;
            uint8_t alpha = app->decoded_pixels[app->curr_row][app->curr_col].alpha;
            
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
