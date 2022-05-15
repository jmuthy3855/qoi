#ifndef QOI_H
#define QOI_H

#include <stdint.h>

#define PREV_PIXELS_LENGTH  64
#define HEADER_SIZE         14

typedef enum qoi_tag_enum {
    QOI_OP_RGB = 0b11111110,
    QOI_OP_RGBA = 0b11111111,
    QOI_OP_INDEX = 0b00,
    QOI_OP_DIFF = 0b01,
    QOI_OP_LUMA = 0b10,
    QOI_OP_RUN = 0b11
} qoi_tag;

typedef struct _pixel_struct_ {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} pixel_struct;

typedef struct _qoi_header_struct_ {
    char magic[4]; // magic bytes "qoif"
    uint32_t width; // image width in pixels (BE)
    uint32_t height; // image height in pixels (BE)
    uint8_t channels; // 3 = RGB, 4 = RGBA
    uint8_t colorspace; // 0 = sRGB with linear alpha
    // 1 = all channels linear
} qoi_header_struct;

typedef struct _qoi_app_struct_ {
    FILE                                *f_qoi;
    char                                *file_buf; // used to load entire QOI file
    long                                file_buf_size;
    qoi_header_struct                   header;

    pixel_struct                        prev_pixels[PREV_PIXELS_LENGTH];
    pixel_struct                        prev_pixel;
    pixel_struct                        *decoded_pixels; // where decoded pixels are stored
    
    int                                 curr_row;
    int                                 curr_col;
    int                                 num_pixels;
} qoi_app_struct;

int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2);
void set_pixel(pixel_struct *pixel, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
void decode_qoi(qoi_app_struct *app);

#endif