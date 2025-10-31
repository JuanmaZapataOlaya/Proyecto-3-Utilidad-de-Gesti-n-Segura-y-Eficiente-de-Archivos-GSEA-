#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/compress.h"

/*
 Simple RLE for bytes.
 Format in output:
   [count:1byte][value:1byte] repeated.
 count is in range 1..255. If a run >255, it is split.
 This is very simple and reversible.
*/

#define BUF 65536

int rle_compress(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "rb");
    if (!fin) { perror("fopen input"); return 1; }
    FILE *fout = fopen(output_path, "wb");
    if (!fout) { perror("fopen output"); fclose(fin); return 1; }

    int c = fgetc(fin);
    if (c == EOF) { fclose(fin); fclose(fout); return 0; }

    unsigned char prev = (unsigned char)c;
    unsigned int count = 1;

    while ((c = fgetc(fin)) != EOF) {
        unsigned char cur = (unsigned char)c;
        if (cur == prev && count < 255) {
            count++;
        } else {
            unsigned char cc = (unsigned char) count;
            if (fwrite(&cc, 1, 1, fout) != 1 || fwrite(&prev, 1, 1, fout) != 1) {
                perror("fwrite");
                fclose(fin); fclose(fout);
                return 1;
            }
            prev = cur;
            count = 1;
        }
    }

    // write last
    unsigned char cc = (unsigned char) count;
    if (fwrite(&cc, 1, 1, fout) != 1 || fwrite(&prev, 1, 1, fout) != 1) {
        perror("fwrite");
        fclose(fin); fclose(fout);
        return 1;
    }

    fclose(fin); fclose(fout);
    return 0;
}

int rle_decompress(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "rb");
    if (!fin) { perror("fopen input"); return 1; }
    FILE *fout = fopen(output_path, "wb");
    if (!fout) { perror("fopen output"); fclose(fin); return 1; }

    int c1, c2;
    while ((c1 = fgetc(fin)) != EOF) {
        c2 = fgetc(fin);
        if (c2 == EOF) {
            fprintf(stderr, "Malformed RLE file: odd number of bytes\n");
            fclose(fin); fclose(fout);
            return 1;
        }
        unsigned char count = (unsigned char)c1;
        unsigned char val = (unsigned char)c2;
        for (unsigned int i = 0; i < count; ++i) {
            if (fputc(val, fout) == EOF) {
                perror("fputc");
                fclose(fin); fclose(fout);
                return 1;
            }
        }
    }

    fclose(fin); fclose(fout);
    return 0;
}
