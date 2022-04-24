#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "main.h"
#include "test.h"

#define TEST_MODE 1

static void print_header(qoi_header_struct *header);

void allocate_pixel_2D_array(pixel_struct ***grid, int width, int height) {
    pixel_struct default_pixel = {0, 0, 0, 255};
    *grid = malloc(sizeof(pixel_struct *) * height);

    if (!*grid) {
        fprintf(stderr, "row malloc failed\n");
        exit(-1);
    }
    
    for (int i = 0; i < height; i++) {
        (*grid)[i] = malloc(sizeof(pixel_struct) * width);

        if (!(*grid)[i]) {
            fprintf(stderr, "column malloc failed\n");
            exit(-1);
        }

        // set row to default pixel color
        set_pixel_row((*grid)[i], width, &default_pixel);
    }
    
}

static void print_header(qoi_header_struct *header) {
    fprintf(stderr, "width: %" PRIu32 "\n", header->width);
    fprintf(stderr, "height: %" PRIu32 "\n", header->height);
    fprintf(stderr, "channels: %" PRIu8 "\n", header->channels);
    fprintf(stderr, "colorspace: %" PRIu8 "\n", header->colorspace);
}

int init_app(qoi_app_struct *app) {
    memset(app, 0, sizeof(*app));
    set_pixel(&app->prev_pixel, 0, 0, 0, 255);

    for (int i = 0; i < PREV_PIXELS_LENGTH; i++) {
        set_pixel(&app->prev_pixels[i], 0, 0, 0, 255);
    }
}

// read header
// width and height are read in reverse order...
// so have to read into uin32_t backwards
void read_header(FILE *fp, qoi_header_struct *header) {
    uint8_t *conv_ptr;
    char buf[4];

    fread(buf, 4, 1, fp);
    conv_ptr = (uint8_t *) &header->width;
    for (int i = 0; i < 4; i++) {
        conv_ptr[i] = buf[3 - i];
    }

    fread(buf, 4, 1, fp);
    conv_ptr = (uint8_t *) &header->height;
    for (int i = 0; i < 4; i++) {
        conv_ptr[i] = buf[3 - i];
    }

    fread(&header->channels, 1, 1, fp);
    fread(&header->colorspace, 1, 1, fp);
}



// check extension and magic bytes, return opened file
FILE *verify_and_open_file(char *fname) {
    FILE *fp;
    char magic_bytes[5];
    char *ext;
    
    ext = strstr(fname, ".qoi");

    if (ext && !strcmp(ext, ".qoi")) {
        fp = fopen(fname, "r");

        if (fp) {
            fread(magic_bytes, 1, 4, fp);
            magic_bytes[4] = '\0';

            if (!strcmp(magic_bytes, "qoif")) {
                fprintf(stderr, "detected QOI input file\n");
                return fp;
            } 
        }
    } 

    return NULL;
}

int main(int argc, char **argv) {
    qoi_app_struct app;

#if TEST_MODE
    fprintf(stderr, "TEST MODE\n");
    test_all();
    return 0;
#endif
    
    // read magic bytes and open file
    app.f_qoi = verify_and_open_file(argv[1]);

    if (!app.f_qoi) {
        fprintf(stderr, "something went wrong when opening input file\n");
        return -1;
    }

    // read header and display
    read_header(app.f_qoi, &app.header);
    print_header(&app.header);

    // allocate memory for decoded pixels 2D array
    allocate_pixel_2D_array(&app.decoded_pixels, app.header.width, app.header.height);


    fprintf(stderr, "QOI decoding done\n");
    return 0;
}