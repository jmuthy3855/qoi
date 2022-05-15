#include <stdio.h>
#include <stdlib.h>
#include "qoi.h"

// debug
static void print_pixel(pixel_struct *pixel);

// post decoding functions
static void post_decode_update(qoi_app_struct *app, qoi_tag qoi_op);
static void store_pixel(qoi_app_struct *app, pixel_struct *pixel);

// QOI OPs
static void handle_op_index(qoi_app_struct *app, int index);
static void handle_op_run(qoi_app_struct *app, uint8_t run_length);
static void handle_op_diff(qoi_app_struct *app, uint8_t red_diff, uint8_t green_diff, uint8_t blue_diff);
static void handle_op_luma(qoi_app_struct *app, uint8_t diff_green, uint8_t diff_red_green, uint8_t diff_blue_green);
static void handle_op_rgb(qoi_app_struct *app, uint8_t red, uint8_t green, uint8_t blue);
static void handle_op_rgba(qoi_app_struct *app, int8_t red, int8_t green, int8_t blue, int8_t alpha);

static void print_pixel(pixel_struct *pixel) {
    fprintf(stderr, "red: %d\n", pixel->red);
    fprintf(stderr, "green: %d\n", pixel->green);
    fprintf(stderr, "blue: %d\n", pixel->blue);
    fprintf(stderr, "alpha: %d\n", pixel->alpha);
}

// called after every QOI op to update app state
static void post_decode_update(qoi_app_struct *app, qoi_tag qoi_op) {
    /* update current pixel */
    app->decoded_pixels[app->num_pixels++] = app->prev_pixel; 
    
    /* store current pixel in prev pixel array */
    store_pixel(app, &app->prev_pixel);
}

static void store_pixel(qoi_app_struct *app, pixel_struct *pixel) {
    int index = (pixel->red * 3 + pixel->green * 5 + pixel->blue * 7 + pixel->alpha * 11) % 64;
    app->prev_pixels[index] = *pixel;
}

// index into array and set as decoded pixel
static void handle_op_index(qoi_app_struct *app, int index) {
    app->prev_pixel = app->prev_pixels[index];
    post_decode_update(app, QOI_OP_INDEX);
}

// run of previous pixels, run length store with bias of -1
static void handle_op_run(qoi_app_struct *app, uint8_t run_length) {
    run_length += 1;

    for (int i = 0; i < run_length; i++) {
        // we just update the current pixel with the previous pixel value
        post_decode_update(app, QOI_OP_RUN);
    }   
}

// rgb diff of previous pixel, store with bias of 2
static void handle_op_diff(qoi_app_struct *app, uint8_t red_diff, uint8_t green_diff, uint8_t blue_diff) {
    app->prev_pixel.red += red_diff - 2;
    app->prev_pixel.green += green_diff - 2;
    app->prev_pixel.blue += blue_diff - 2;
    // alpha unchanged
    post_decode_update(app, QOI_OP_DIFF);
}

// green diffs and red/blue diffs relative to green diffs, green diff has bias of 32 and red/blue diffs have bias of 8
static void handle_op_luma(qoi_app_struct *app, uint8_t diff_green, uint8_t diff_red_green, uint8_t diff_blue_green) {
    diff_green -= 32;

    // compute current green val
    app->prev_pixel.green += diff_green;

    // some simple arithmetic
    app->prev_pixel.red += diff_green - 8 + diff_red_green;
    app->prev_pixel.blue += diff_green - 8 + diff_blue_green;
    // alpha unchanged
    post_decode_update(app, QOI_OP_LUMA);
}

// rgb values encoded, simply extract and store
static void handle_op_rgb(qoi_app_struct *app, uint8_t red, uint8_t green, uint8_t blue) {
    app->prev_pixel.red = red;
    app->prev_pixel.green = green;
    app->prev_pixel.blue = blue;
    // alpha unchanged
    post_decode_update(app, QOI_OP_RGB);
}

// same as rgb but with alpha component
static void handle_op_rgba(qoi_app_struct *app, int8_t red, int8_t green, int8_t blue, int8_t alpha) {
    app->prev_pixel.red = red;
    app->prev_pixel.green = green;
    app->prev_pixel.blue = blue;
    app->prev_pixel.alpha = alpha;
    post_decode_update(app, QOI_OP_RGBA);
}

int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2) {
    return (pixel_1->red == pixel_2->red && pixel_1->green == pixel_2->green &&
            pixel_1->blue == pixel_2->blue && pixel_1->alpha == pixel_2->alpha);
}

void set_pixel(pixel_struct *pixel, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    pixel->red = red;
    pixel->green = green;
    pixel->blue = blue;
    pixel->alpha = alpha;
}

void decode_qoi(qoi_app_struct *app) {
    char *file_buf = app->file_buf;
    int file_buf_ptr = 0;
    int total_pixels = app->header.width * app->header.height;

    while (app->num_pixels < total_pixels)  {
        if ((file_buf[file_buf_ptr] & 0xFF) == QOI_OP_RGB) {
            handle_op_rgb(app, file_buf[file_buf_ptr + 1], file_buf[file_buf_ptr + 2], file_buf[file_buf_ptr + 3]);
            file_buf_ptr += 4;
        } else if ((file_buf[file_buf_ptr] & 0xFF) == QOI_OP_RGBA) {
            handle_op_rgba(app, file_buf[file_buf_ptr + 1], file_buf[file_buf_ptr + 2], file_buf[file_buf_ptr + 3], file_buf[file_buf_ptr + 4]);
            file_buf_ptr += 5;
        } else if ((file_buf[file_buf_ptr] & 0xC0) == QOI_OP_INDEX) {
            handle_op_index(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_RUN) {
            handle_op_run(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_DIFF) {
            handle_op_diff(app, (file_buf[file_buf_ptr] & 0x30) >> 4, (file_buf[file_buf_ptr] & 0x0C) >> 2, file_buf[file_buf_ptr] & 0x03);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_LUMA) {
            handle_op_luma(app, file_buf[file_buf_ptr] & 0x3F, (file_buf[file_buf_ptr + 1] >> 4) & 0x0F, file_buf[file_buf_ptr + 1] & 0x0F);
            file_buf_ptr += 2;
        } else {
            fprintf(stderr, "invalid tag, stopped at address 0x%X byte: 0x%hhX\n", file_buf_ptr + HEADER_SIZE, file_buf[file_buf_ptr]);
            exit(-1);
        }
    }
}