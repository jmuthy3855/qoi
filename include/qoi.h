#ifndef QOI_H
#define QOI_H

#include <stdint.h>

typedef struct _qoi_header_struct_ {
    char magic[4]; // magic bytes "qoif"
    uint32_t width; // image width in pixels (BE)
    uint32_t height; // image height in pixels (BE)
    uint8_t channels; // 3 = RGB, 4 = RGBA
    uint8_t colorspace; // 0 = sRGB with linear alpha
    // 1 = all channels linear
} qoi_header_struct;

typedef struct _pixel_struct_ {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} pixel_struct;

int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2);
void set_pixel(pixel_struct *pixel, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
void set_pixel_row(pixel_struct *row, int row_length, pixel_struct *pixel);

#endif