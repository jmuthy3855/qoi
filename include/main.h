#ifndef MAIN_H
#define MAIN_H

#include "qoi.h"

int init_app(qoi_app_struct *app);
void allocate_pixel_2D_array(pixel_struct ***grid, int width, int height);
void read_header(FILE *fp, qoi_header_struct *header);
FILE *verify_and_open_file(char *fname);

#endif