#ifndef MAIN_H
#define MAIN_H

#define PREV_PIXELS_LENGTH 64

#include <stdio.h>
#include "qoi.h"

typedef struct _qoi_app_struct_ {
    FILE                                *f_qoi;
    qoi_header_struct                   header;

    pixel_struct                        prev_pixels[PREV_PIXELS_LENGTH];
    pixel_struct                        prev_pixel;
    pixel_struct                        **decoded_pixels; // where decoded pixels are stored
    
    int                                 curr_row;
    int                                 curr_col;
} qoi_app_struct;

int init_app(qoi_app_struct *app);
void allocate_pixel_2D_array(pixel_struct ***grid, int width, int height);
void read_header(FILE *fp, qoi_header_struct *header);
FILE *verify_and_open_file(char *fname);


#endif