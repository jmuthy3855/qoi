#include <stdio.h>
#include <stdlib.h>
#include "qoi.h"

// debug
static void print_pixel(pixel_struct *pixel);

// post decoding functions
static void store_pixel(qoi_app_struct *app, pixel_struct *pixel);

// QOI OPs
static void handle_op_index(qoi_app_struct *app, int index);
static void handle_op_run(qoi_app_struct *app, uint8_t run_length);
static void handle_op_diff(qoi_app_struct *app, uint8_t r_diff, uint8_t g_diff, uint8_t b_diff);
static void handle_op_luma(qoi_app_struct *app, uint8_t g_diff, uint8_t r_g_diff, uint8_t b_g_diff);
static void handle_op_rgb(qoi_app_struct *app, uint8_t r, uint8_t g, uint8_t b);
static void handle_op_rgba(qoi_app_struct *app, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

static void print_pixel(pixel_struct *pixel) {
    fprintf(stderr, "red: %d\n", pixel->r);
    fprintf(stderr, "green: %d\n", pixel->g);
    fprintf(stderr, "blue: %d\n", pixel->b);
    fprintf(stderr, "alpha: %d\n", pixel->a);
}

static void store_pixel(qoi_app_struct *app, pixel_struct *pixel) {
    int index = (pixel->r * 3 + pixel->g * 5 + pixel->b * 7 + pixel->a * 11) % 64;
    app->prev_pixels[index] = *pixel;
}

// index into array and set as decoded pixel
static void handle_op_index(qoi_app_struct *app, int index) {
    app->prev_pixel = app->prev_pixels[index];
}

// run of previous pixels, run length store with bias of -1
// bias accounted for by post decode
static void handle_op_run(qoi_app_struct *app, uint8_t run_length) {
    for (int i = 0; i < run_length; i++) {
        app->decoded_pixels[app->num_pixels++] = app->prev_pixel;
    }
}

// rgb diff of previous pixel, store with bias of 2
static void handle_op_diff(qoi_app_struct *app, uint8_t r_diff, uint8_t g_diff, uint8_t b_diff) {
    app->prev_pixel.r += r_diff - 2;
    app->prev_pixel.g += g_diff - 2;
    app->prev_pixel.b += b_diff - 2;
}

// green diffs and red/blue diffs relative to green diffs, green diff has bias of 32 and red/blue diffs have bias of 8
// alpha unchanged
static void handle_op_luma(qoi_app_struct *app, uint8_t g_diff, uint8_t r_g_diff, uint8_t b_g_diff) {
    g_diff -= 32;

    // compute current green val
    app->prev_pixel.g += g_diff;

    // some simple arithmetic
    app->prev_pixel.r += g_diff - 8 + r_g_diff;
    app->prev_pixel.b += g_diff - 8 + b_g_diff;
}

// rgb values encoded, alpha unchanged
static void handle_op_rgb(qoi_app_struct *app, uint8_t r, uint8_t g, uint8_t b) {
    app->prev_pixel.r = r;
    app->prev_pixel.g = g;
    app->prev_pixel.b = b;
}

// same as rgb but with alpha component
static void handle_op_rgba(qoi_app_struct *app, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    app->prev_pixel.r = r;
    app->prev_pixel.g = g;
    app->prev_pixel.b = b;
    app->prev_pixel.a = a;
}

int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2) {
    return (pixel_1->r == pixel_2->r && pixel_1->g == pixel_2->g &&
            pixel_1->b == pixel_2->b && pixel_1->a == pixel_2->a);
}

void set_pixel(pixel_struct *pixel, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    pixel->r = red;
    pixel->g = green;
    pixel->b = blue;
    pixel->a = alpha;
}

void decode_qoi(qoi_app_struct *app) {
    char *buf = app->file_buf;
    int buf_ptr = 0;
    const int total_pixels = app->header.width * app->header.height;

    while (app->num_pixels < total_pixels)  {
        if ((buf[buf_ptr] & 0xFF) == QOI_OP_RGB) {
            handle_op_rgb(app, buf[buf_ptr + 1], buf[buf_ptr + 2], buf[buf_ptr + 3]);
            buf_ptr += 4; // i want this to be moved to end of if else...
        } else if ((buf[buf_ptr] & 0xFF) == QOI_OP_RGBA) {
            handle_op_rgba(app, buf[buf_ptr + 1], buf[buf_ptr + 2], buf[buf_ptr + 3], buf[buf_ptr + 4]);
            buf_ptr += 5;
        } else if ((buf[buf_ptr] & 0xC0) == QOI_OP_INDEX) {
            handle_op_index(app, buf[buf_ptr] & 0x3F);
            buf_ptr += 1;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_RUN) {
            handle_op_run(app, buf[buf_ptr] & 0x3F);
            buf_ptr += 1;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_DIFF) {
            handle_op_diff(app, (buf[buf_ptr] & 0x30) >> 4, (buf[buf_ptr] & 0x0C) >> 2, buf[buf_ptr] & 0x03);
            buf_ptr += 1;
        } else if (((buf[buf_ptr] & 0xC0) >> 6) == QOI_OP_LUMA) {
            handle_op_luma(app, buf[buf_ptr] & 0x3F, (buf[buf_ptr + 1] >> 4) & 0x0F, buf[buf_ptr + 1] & 0x0F);
            buf_ptr += 2;
        } else {
            fprintf(stderr, "invalid tag, stopped at address 0x%X byte: 0x%hhX\n", buf_ptr + HEADER_SIZE, buf[buf_ptr]);
            exit(-1);
        }

        /* note that this conveniently accounts for the -1 bias for the run tag */
        app->decoded_pixels[app->num_pixels++] = app->prev_pixel; 
        store_pixel(app, &app->prev_pixel);
    }
}