#include <stdio.h>
#include <stdlib.h>
#include "qoi.h"

// debug
static void print_pixel(pixel_struct *pixel);

static void print_pixel(pixel_struct *pixel) {
    fprintf(stderr, "red: %d\n", pixel->r);
    fprintf(stderr, "green: %d\n", pixel->g);
    fprintf(stderr, "blue: %d\n", pixel->b);
    fprintf(stderr, "alpha: %d\n", pixel->a);
}

/* QOI_OP_RGB and QOI_OP_RGBA can be combined into one related case,
   but separating them makes it more readable */
void decode_qoi(qoi_app_struct *app) {
    uint8_t *buf             = (uint8_t *) app->file_buf;
    int buf_ptr              = 0;
    int prev_pixels_index    = 0;
    int run_count            = 0;
    const int total_pixels   = app->header.width * app->header.height;
    
    /* decoded values (r,g,b,a already in prev_pixel) */
    uint8_t run_length       = 0;
    uint8_t index            = 0;
    uint8_t r_diff           = 0;
    uint8_t g_diff           = 0;
    uint8_t b_diff           = 0; 
    uint8_t r_g_diff         = 0;
    uint8_t b_g_diff         = 0;

    while (app->num_pixels < total_pixels)  {
        if (run_length != 0) {
            run_length--;
        }  else if ((buf[buf_ptr] & 0xFF) == QOI_OP_RGB) {
            app->prev_pixel.r = buf[++buf_ptr];
            app->prev_pixel.g = buf[++buf_ptr];
            app->prev_pixel.b = buf[++buf_ptr];
            buf_ptr++;
        } else if ((buf[buf_ptr] & 0xFF) == QOI_OP_RGBA) {
            app->prev_pixel.r = buf[++buf_ptr];
            app->prev_pixel.g = buf[++buf_ptr];
            app->prev_pixel.b = buf[++buf_ptr];
            app->prev_pixel.a = buf[++buf_ptr];
            buf_ptr++;
        } else if ((buf[buf_ptr] & 0xC0) == QOI_OP_INDEX) {
            index = buf[buf_ptr] & 0x3F;
            app->prev_pixel = app->prev_pixels[index];
            buf_ptr++;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_RUN) {
            run_length = (buf[buf_ptr] & 0x3F) + 1;
            buf_ptr++;
            continue;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_DIFF) {
            r_diff = (buf[buf_ptr] & 0x30) >> 4;
            g_diff = (buf[buf_ptr] & 0x0C) >> 2;
            b_diff = (buf[buf_ptr] & 0x03);

            app->prev_pixel.r += r_diff - 2;
            app->prev_pixel.g += g_diff - 2;
            app->prev_pixel.b += b_diff - 2;
            buf_ptr++;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_LUMA) {
            g_diff   = (buf[buf_ptr++] & 0x3F) - 32;
            r_g_diff = (buf[buf_ptr] >> 4) & 0x0F;
            b_g_diff = buf[buf_ptr] & 0x0F;
            
            // compute current green val
            app->prev_pixel.g += g_diff;

            // some simple arithmetic
            app->prev_pixel.r += g_diff - 8 + r_g_diff;
            app->prev_pixel.b += g_diff - 8 + b_g_diff;
            buf_ptr += 1;
        } else {
            fprintf(stderr, "invalid tag, stopped at address 0x%X byte: 0x%hhX\n", buf_ptr + HEADER_SIZE, buf[buf_ptr]);
            exit(-1);
        }

        app->decoded_pixels[app->num_pixels++] = app->prev_pixel; 
        prev_pixels_index = (app->prev_pixel.r * 3 + app->prev_pixel.g * 5 + app->prev_pixel.b * 7 + app->prev_pixel.a * 11) % 64;
        app->prev_pixels[prev_pixels_index] = app->prev_pixel;
    }
}