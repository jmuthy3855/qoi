#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "test.h"

#define TEST_MODE 1

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
    FILE *f_qoi;

#if TEST_MODE
    fprintf(stderr, "TEST MODE\n");
    test_all();
#endif
    
    f_qoi = verify_and_open_file(argv[1]);

    if (!f_qoi) {
        fprintf(stderr, "something went wrong when opening input file\n");
        exit(-1);
    }

    fprintf(stderr, "done\n");
    return 0;
}