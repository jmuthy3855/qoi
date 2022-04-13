#include <stdio.h>
#include <assert.h>
#include "test.h"
#include "main.h"

static void test_verify(void);

static char *input_files[7] = {"dice", "kodim10", "kodim23", "qoi_logo", "testcard", "testcard_rgba", "wikipedia_008"};

void test_all(void) {
    test_verify();

    fprintf(stderr, "all tests passed!\n");
}

static void test_verify(void) {
    FILE *curr;
    char fname[50];

    for (int i = 0; i < 7; i++) {
        snprintf(fname, 50, "..\\qoi_test_images\\%s.qoi", input_files[i]);
        //fprintf(stderr, "%s\n", fname);
        curr = verify_and_open_file(fname);
        assert(curr != NULL);
    }

    fprintf(stderr, "test_verify passed\n");
}