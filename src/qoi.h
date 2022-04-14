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

#endif