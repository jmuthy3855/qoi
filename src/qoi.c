#include <stdio.h>
#include <stdlib.h>
#include "qoi.h"

static void print_debug(qoi_app_struct *app);
static void update_row_and_col(int *row, int *col, int max_width);
static void handle_op_index(qoi_app_struct *app, int index);
static void handle_op_run(qoi_app_struct *app, int run_length);

static void print_debug(qoi_app_struct *app) {
    for (int i = 0; i < 10; i++) {
        fprintf(stderr, "r: %d ", app->decoded_pixels[0][i].red);
        fprintf(stderr, "g: %d ", app->decoded_pixels[0][i].green);
        fprintf(stderr, "b: %d ", app->decoded_pixels[0][i].blue);
        fprintf(stderr, "a: %d ", app->decoded_pixels[0][i].alpha);
        fprintf(stderr, "|");
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "curr_row: %d\n", app->curr_row);
    fprintf(stderr, "curr_col: %d\n", app->curr_col);
    
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

    // update previous pixel value
    app->prev_pixel = app->decoded_pixels[app->curr_row][app->curr_col];

    // update row and col
    update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
}

static void handle_op_run(qoi_app_struct *app, int run_length) {
    run_length += 1; // run length stored with bias of -1

    for (int i = 0; i < run_length; i++) {
        app->decoded_pixels[app->curr_row][app->curr_col] = app->prev_pixel;
        update_row_and_col(&app->curr_row, &app->curr_col, app->header.width);
    }

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

    for (int i = 0; i < 100000; i++) {
        if ((file_buf[file_buf_ptr] & 0xC0) == QOI_OP_INDEX) {
            fprintf(stderr, "found index tag\n");
            handle_op_index(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else if (((file_buf[file_buf_ptr] & 0xC0) >> 6) == QOI_OP_RUN) {
            fprintf(stderr, "found run tag\n");
            handle_op_run(app, file_buf[file_buf_ptr] & 0x3F);
            file_buf_ptr += 1;
        } else {
            fprintf(stderr, "invalid tag, stopped at address 0x%X\n", file_buf_ptr);
            break;
        }
    }
    
    //print_debug(app);
}