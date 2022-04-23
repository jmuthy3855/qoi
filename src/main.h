#ifndef MAIN_H
#define MAIN_H

#define PREV_PIXELS_LENGTH 64

#include <stdio.h>
#include "qoi.h"

typedef struct _qoi_app_struct_ {
    FILE *f_qoi;
    qoi_header_struct header;
    pixel_struct prev_pixels[PREV_PIXELS_LENGTH];
    pixel_struct prev_pixel;
} qoi_app_struct;

int init_app(qoi_app_struct *app);
void read_header(FILE *fp, qoi_header_struct *header);
FILE *verify_and_open_file(char *fname);


#endif