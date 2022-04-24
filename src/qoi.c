#include "qoi.h"
#include "main.h"

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