#ifndef COMPRESSION_H
#define COMPRESSION_H

int rle_compress(const char *input_path, const char *output_path);
int rle_decompress(const char *input_path, const char *output_path);

#endif // COMPRESSION_H
