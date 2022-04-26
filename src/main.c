#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "main.h"
#include "qoi.h"
#include "display_qoi.h"
#include "test.h"

#define TEST_MODE       0
#define READ_SIZE       4096

static long alloc_and_load_into_filebuf(char **file_buf, FILE *fp);
static void print_header(qoi_header_struct *header);

// allocate and load file into buffer, assumes QOI magic bytes and header already read
static long alloc_and_load_into_filebuf(char **file_buf, FILE *fp) {
    size_t total_bytes_read = 0;
    int i = 0;
    char *save_ptr;

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp) - HEADER_SIZE; // we already read header
    fseek(fp, HEADER_SIZE, SEEK_SET);

    *file_buf = malloc(fsize);

    if (!*file_buf) {
        fprintf(stderr, "filebuf malloc error\n");
        exit(-1);
    }

    save_ptr = *file_buf;

    size_t bytes_read = fread(*file_buf, 1, fsize, fp); // is reading in loop better??
    total_bytes_read += bytes_read;
    *file_buf = save_ptr;

    return fsize;
}

static void print_header(qoi_header_struct *header) {
    fprintf(stderr, "width: %" PRIu32 "\n", header->width);
    fprintf(stderr, "height: %" PRIu32 "\n", header->height);
    fprintf(stderr, "channels: %" PRIu8 "\n", header->channels);
    fprintf(stderr, "colorspace: %" PRIu8 "\n", header->colorspace);
}

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
    }
}

int init_app(qoi_app_struct *app) {
    memset(app, 0, sizeof(*app));
    set_pixel(&app->prev_pixel, 0, 0, 0, 255);

    for (int i = 0; i < PREV_PIXELS_LENGTH; i++) {
        set_pixel(&app->prev_pixels[i], 0, 0, 0, 255); // 255 or 0?
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
        // without the b, fread reaches EOF after 712 bytes...
        fp = fopen(fname, "r+b");

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

    if (argc < 2) {
        fprintf(stderr, "input file not specified\n");
        return -1;
    }

    init_app(&app);
    
    // read magic bytes and open file
    app.f_qoi = verify_and_open_file(argv[1]);

    if (!app.f_qoi) {
        fprintf(stderr, "something went wrong when opening input file\n");
        return -1;
    }

    // read header and display
    read_header(app.f_qoi, &app.header);
    print_header(&app.header);

    // allocate file buf, load entire file contents into filebuf
    app.file_buf_size = alloc_and_load_into_filebuf(&app.file_buf, app.f_qoi);
    fprintf(stderr, "QOI encoded bytes read: %ld\n", app.file_buf_size);
    fclose(app.f_qoi);

    // allocate memory for decoded pixels 2D array
    allocate_pixel_2D_array(&app.decoded_pixels, app.header.width, app.header.height);

    // decode qoi image
    decode_qoi(&app);

    // print qoi image
    print_qoi(&app);

    fprintf(stderr, "QOI decoding done\n");
    return 0;
}