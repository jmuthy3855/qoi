#include <stdio.h>
#include <stdlib.h>
#include "qoi.h"

static void print_pixel(pixel_struct *pixel);
static void store_pixel(qoi_app_struct *app, pixel_struct *pixel);
static void update_row_and_col(int *row, int *col, int max_width);
static void handle_op_index(qoi_app_struct *app, int index);
static void handle_op_run(qoi_app_struct *app, int run_length);
static void handle_op_diff(qoi_app_struct *app, int red_diff, int green_diff, int blue_diff);
static void handle_op_luma(qoi_app_struct *app, int diff_green, int diff_red_green, int diff_blue_green);
static void handle_op_rgb(qoi_app_struct *app, int red, int green, int blue);
static void handle_op_rgba(qoi_app_struct *app, int red, int green, int blue, int alpha);

static void print_pixel(pixel_struct *pixel) {
    fprintf(stderr, "red: %d\n", pixel->red);
    fprintf(stderr, "green: %d\n", pixel->green);
    fprintf(stderr, "blue: %d\n", pixel->blue);
    fprintf(stderr, "alpha: %d\n", pixel->alpha);
}

static void store_pixel(qoi_app_struct *app, pixel_struct *pixel) {
    int index = ( (pixel->red * 3) + (pixel->green * 5) + (pixel->blue * 7) + (pixel->alpha * 11)) % 64;
    app->prev_pixels[index] = *pixel;
}

static void update_row_and_col(int *row, int *col, int max_width) {
    *col = (*col + 1) % max_width;

    if (*col == 0) {
        *row += 1;
    }
}

static void handle_op_index(qoi_app_struct *app, int index) {
    // index into array and set as decoded pixel
    app->decoded_pixels[app->curr_row][app->curr_col] = app->prev_pixels[index];

    // update prev pixel, prev array, row and col, num pixels
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
    store_pixel(app, &app->prev_pixel);
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    app->num_pixels++;
}

static void handle_op_run(qoi_app_struct *app, int run_length) {
    run_length += 1; // run length stored with bias of -1

    for (int i = 0; i < run_length; i++) {
        // use previous pixel for run
        app->decoded_pixels[app->curr_row][app->curr_col] = app->prev_pixel;

        app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
        store_pixel(app, &app->prev_pixel);
        update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
        app->num_pixels += 1;
    }

    // we only need to store the previous pixel in the prev array once
    // no need to update prev_pixel, it's the same
    
   // app->num_pixels += run_length;
   
}

static void handle_op_diff(qoi_app_struct *app, int red_diff, int green_diff, int blue_diff) {
    app->decoded_pixels[app->curr_row][app->curr_col] = app->prev_pixel;
    
    // bias of 2
    red_diff -= 2;
    green_diff -= 2;
    blue_diff -= 2;
    
    app->decoded_pixels[app->curr_row][app->curr_col].red = app->prev_pixel.red + red_diff;
    app->decoded_pixels[app->curr_row][app->curr_col].green = app->prev_pixel.green + green_diff;
    app->decoded_pixels[app->curr_row][app->curr_col].blue = app->prev_pixel.blue + blue_diff;
    app->decoded_pixels[app->curr_row][app->curr_col].alpha = app->prev_pixel.alpha;

    // update prev pixel, prev array, row and col, num pixels
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
    store_pixel(app, &app->prev_pixel);
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    app->num_pixels++;
}

static void handle_op_luma(qoi_app_struct *app, int diff_green, int diff_red_green, int diff_blue_green) {
    int8_t red_diff;
    int8_t blue_diff;
    
    diff_green -= 32;
    //diff_red_green -= 8;
    //diff_blue_green -= 8;

    // compute current green val
    app->decoded_pixels[app->curr_row][app->curr_col].green = app->prev_pixel.green + diff_green;

    // confirm with some basic arithmetic
    app->decoded_pixels[app->curr_row][app->curr_col].red = app->prev_pixel.red + diff_green - 8 + diff_red_green;
    app->decoded_pixels[app->curr_row][app->curr_col].blue = app->prev_pixel.blue + diff_green - 8 + diff_blue_green;
    app->decoded_pixels[app->curr_row][app->curr_col].alpha = app->prev_pixel.alpha;

    // update prev pixel, prev array, row and col, num pixels
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
    store_pixel(app, &app->prev_pixel);
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    app->num_pixels++;
}

static void handle_op_rgb(qoi_app_struct *app, int red, int green, int blue) {
    app->decoded_pixels[app->curr_row][app->curr_col].red = red;
    app->decoded_pixels[app->curr_row][app->curr_col].green = green;
    app->decoded_pixels[app->curr_row][app->curr_col].blue = blue;
    app->decoded_pixels[app->curr_row][app->curr_col].alpha = app->prev_pixel.alpha;

    // update prev pixel, prev array, row and col, num pixels
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
    store_pixel(app, &app->prev_pixel);
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    app->num_pixels++;
}

static void handle_op_rgba(qoi_app_struct *app, int red, int green, int blue, int alpha) {
    app->decoded_pixels[app->curr_row][app->curr_col].red = red;
    app->decoded_pixels[app->curr_row][app->curr_col].green = green;
    app->decoded_pixels[app->curr_row][app->curr_col].blue = blue;
    app->decoded_pixels[app->curr_row][app->curr_col].alpha = alpha;

    // update prev pixel, prev array, row and col, num pixels
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];
    store_pixel(app, &app->prev_pixel);
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    app->num_pixels++;
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

// row should be array
void set_pixel_row(pixel_struct *row, int row_length, pixel_struct *pixel) {
    for (int i = 0; i < row_length; i++) {
        row[i] = *pixel;
    }
}


void decode_qoi(qoi_app_struct *app) {
    char *file_buf = app->file_buf;
    int file_buf_ptr = 0;
    int total_pixels = app->header.width * app->header.height; // what

    while (app->num_pixels < total_pixels)  {

        if ((file_buf[file_buf_ptr] & 0xFF) == QOI_OP_RGB) {
            //fprintf(stderr, "found rgb tag\n");
            handle_op_rgb(app, file_buf[file_buf_ptr + 1], file_buf[file_buf_ptr + 2], file_buf[file_buf_ptr + 3]);
            file_buf_ptr += 4;
        } else if ((file_buf[file_buf_ptr] & 0xFF) == QOI_OP_RGBA) {
            //fprintf(stderr, "found rgba tag\n");
            handle_op_rgba(app, file_buf[file_buf_ptr + 1], file_buf[file_buf_ptr + 2], file_buf[file_buf_ptr + 3], file_buf[file_buf_ptr + 4]);
            file_buf_ptr += 5;
        } else if ((file_buf[file_buf_ptr] & 0xC0) == QOI_OP_INDEX) {
            //fprintf(stderr, "found index tag\n");
            handle_op_index(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_RUN) {
           // fprintf(stderr, "found run tag\n");
            handle_op_run(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_DIFF) {
           // fprintf(stderr, "found diff tag\n");
            handle_op_diff(app, (file_buf[file_buf_ptr] & 0x30) >> 4, (file_buf[file_buf_ptr] & 0x0C) >> 2, file_buf[file_buf_ptr] & 0x03);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_LUMA) {
          //  fprintf(stderr, "found luma tag\n");
            handle_op_luma(app, file_buf[file_buf_ptr] & 0x3F, (file_buf[file_buf_ptr + 1] >> 4) & 0x0F, file_buf[file_buf_ptr + 1] & 0x0F);
            file_buf_ptr += 2;
        } else {
            fprintf(stderr, "invalid tag, stopped at address 0x%X byte: 0x%hhX\n", file_buf_ptr + HEADER_SIZE, file_buf[file_buf_ptr]);
            break; 
        }

        //fprintf(stderr, "num of pixels: %d\n", app->num_pixels);
    }

    fprintf(stderr, "curr_row: %d\n", app->curr_row);
    fprintf(stderr, "curr_col: %d\n", app->curr_col);
    fprintf(stderr, "total pixels: %d\n", app->num_pixels);
    //print_pixel(&app->decoded_pixels[app->curr_row][app->curr_col]);
    
    //print_debug(app);
}