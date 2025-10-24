#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void vigenere_encrypt(const char *input_path, const char *output_path, const char *key);
void vigenere_decrypt(const char *input_path, const char *output_path, const char *key);

#endif