#include <stdio.h>
#include <assert.h>
#include "qoi.h"
#include "test.h"
#include "main.h"

#define NUM_TEST_FILES 7

static int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2);
static void test_verify_and_header(void);
static void test_init_app(void);

// test files provided by QOI website
static char *input_files[NUM_TEST_FILES] = {"dice", "kodim10", "kodim23", "qoi_logo", "testcard", "testcard_rgba", "wikipedia_008"};

// we don't care about magic bytes
static qoi_header_struct headers[NUM_TEST_FILES] = {
    { {0}, 800, 600, 4, 0},
    { {0}, 512, 768, 3, 0},
    { {0}, 768, 512, 3, 0},
    { {0}, 448, 220, 4, 0},
    { {0}, 256, 256, 4, 0},
    { {0}, 256, 256, 4, 0},
    { {0}, 1152, 858, 3, 0}
};

void test_all(void) {
    test_verify_and_header();
    test_init_app();

    fprintf(stderr, "all tests passed!\n");
}

static int pixel_equals(pixel_struct *pixel_1, pixel_struct *pixel_2) {
    return (pixel_1->red == pixel_2->red && pixel_1->green == pixel_2->green &&
            pixel_1->blue == pixel_2->blue && pixel_1->alpha == pixel_2->alpha);
}

// verifying and reading header are related
static void test_verify_and_header(void) {
    FILE *curr;
    char fname[50];
    qoi_header_struct header;

    for (int i = 0; i < NUM_TEST_FILES; i++) {
        snprintf(fname, 50, "..\\qoi_test_images\\%s.qoi", input_files[i]);
        //fprintf(stderr, "%s\n", fname);
        curr = verify_and_open_file(fname);
        assert(curr != NULL);

        read_header(curr, &header);
        assert(header.width == headers[i].width);
        assert(header.height == headers[i].height);
        assert(header.channels == headers[i].channels);
        assert(header.colorspace == headers[i].colorspace);

        fprintf(stderr, "test_verify %d passed\n", i + 1);
    }
}

static void test_init_app(void) {
    qoi_app_struct app;
    pixel_struct cmp = {0, 0, 0, 255};

    init_app(&app);
    assert(pixel_equals(&app.prev_pixel, &cmp));

    for (int i = 0; i < PREV_PIXELS_LENGTH; i++) {
        assert(pixel_equals(&app.prev_pixels[i], &cmp));
    }

    fprintf(stderr, "test_init passed\n");
}