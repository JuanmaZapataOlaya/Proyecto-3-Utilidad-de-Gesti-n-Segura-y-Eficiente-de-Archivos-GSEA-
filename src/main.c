#include "../include/encryption.h"

int main(int argc, char *argv[]) {
    if (argc < 7) {
        printf("Uso: %s -e|-u -i <input> -o <output> -k <clave>\n", argv[0]);
        return 1;
    }

    char *input_path = NULL;
    char *output_path = NULL;
    char *key = NULL;
    int encrypt = 0, decrypt = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0) encrypt = 1;
        else if (strcmp(argv[i], "-u") == 0) decrypt = 1;
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) input_path = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) output_path = argv[++i];
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) key = argv[++i];
    }

    if (!input_path || !output_path || !key) {
        printf("Error: faltan parámetros obligatorios.\n");
        return 1;
    }

    if (encrypt)
        vigenere_encrypt(input_path, output_path, key);
    else if (decrypt)
        vigenere_decrypt(input_path, output_path, key);
    else
        printf("Debe especificar -e (encriptar) o -u (desencriptar)\n");

    return 0;
}
