#include <stdio.h>
#include "display_qoi.h"
#include "SDL.h"
#include "SDL_image.h"
 
void print_qoi(qoi_app_struct *app) {
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Rect texture_rect;
    uint32_t *draw_buffer;
    int draw_buffer_idx = 0;
    pixel_struct **pixel_grid = app->decoded_pixels;

    /* initalize library, window and renderer */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(app->header.width, app->header.height, 0, &window, &renderer);
    
    /* clear screen to all black */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* create texture and set alpha blend mode */    
    texture = SDL_CreateTexture(renderer, 
                                SDL_PIXELFORMAT_RGBA8888, 
                                SDL_TEXTUREACCESS_STREAMING, 
                                app->header.width,
                                app->header.height);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    /* allocate texture draw buffer */
    draw_buffer = malloc(sizeof(uint32_t) * app->header.width * app->header.height);

    /* set texture rectangle location and size, used to position and size texture */
    texture_rect.x = 0;
    texture_rect.y = 0;
    texture_rect.w = app->header.width;
    texture_rect.h = app->header.height;

    /* fill draw buffer with decoded pixel values*/
    for (int r = 0; r < app->header.height; r++) {
        for (int c = 0; c < app->header.width; c++) {
            // white space...?
            draw_buffer[draw_buffer_idx++] = (pixel_grid[r][c].red   << 24) | 
                                             (pixel_grid[r][c].green << 16) | 
                                             (pixel_grid[r][c].blue  <<  8) |  
                                              pixel_grid[r][c].alpha;
        }
    }

    /* update texture with pixels in draw buffer */
    SDL_UpdateTexture(texture , NULL, draw_buffer, app->header.width * sizeof(uint32_t));

    /* copy text to rendering target*/
    SDL_RenderCopy(renderer, texture, NULL, &texture_rect); 
    
    /* update screen */
    SDL_RenderPresent(renderer);
    
    /* display decoded picture until user quits out */
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }
    
    /* clean up */
    free(draw_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}
