#include "../include/encryption.h"
#include "../include/compress.h"

int main(int argc, char *argv[]) {
    if (argc < 7) {
        printf("Uso: %s -e|-u|-c|-d -i  <input> -o <output> -k <clave>\n", argv[0]);
        return 1;
    }

    char *input_path = NULL;
    char *output_path = NULL;
    char *key = NULL;
    int encrypt = 0, decrypt = 0;
    int compress = 0, decompress = 0;
    int contador = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0) encrypt = 1;
        else if (strcmp(argv[i], "-u") == 0) decrypt = 1;
        else if (strcmp(argv[i], "-c") == 0) compress = 1;
        else if (strcmp(argv[i], "-d") == 0) decompress = 1;
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) input_path = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) output_path = argv[++i];
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) key = argv[++i];
    }

    if (!input_path || !output_path || !key) {
        printf("Error: faltan parámetros obligatorios.\n");
        return 1;
    }
    
    if (compress) {
        rle_compress(input_path, output_path);
        contador++;
    }
    else if (decompress) {
        rle_decompress(input_path, output_path);
        contador++;
    }

    if (encrypt) {
        vigenere_encrypt(input_path, output_path, key);
        contador++;
    }
    else if (decrypt) {
        vigenere_decrypt(input_path, output_path, key);
        contador++;
    }

    if (contador < 1) {
        printf("Error: debe especificar una operación (-e, -u, -c, -d).\n");
        return 1;
    }

    return 0;

    
}
