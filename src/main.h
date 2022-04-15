#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "qoi.h"

typedef struct _qoi_app_struct_ {
    FILE *f_qoi;
    qoi_header_struct header;
} qoi_app_struct;

void read_header(FILE *fp, qoi_header_struct *header);
FILE *verify_and_open_file(char *fname);


#endif