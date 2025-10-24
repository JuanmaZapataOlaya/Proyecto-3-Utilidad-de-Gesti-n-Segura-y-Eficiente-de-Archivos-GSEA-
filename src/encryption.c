#include "../include/encryption.h"

// Función auxiliar: aplica el desplazamiento del cifrado
char encrypt_char(char c, char k, int decrypt) {
    if (!isalpha(c)) return c;  // Si no es letra, se deja igual

    char base = isupper(c) ? 'A' : 'a';
    char base_k = isupper(k) ? 'A' : 'a';

    int shift = (k - base_k);
    if (decrypt) shift = 26 - shift;  // Si estamos desencriptando, invertimos

    return (c - base + shift) % 26 + base;
}

// Cifra un archivo completo usando Vigenère
void vigenere_encrypt(const char *input_path, const char *output_path, const char *key) {
    FILE *input = fopen(input_path, "r");
    FILE *output = fopen(output_path, "w");

    if (!input || !output) {
        perror("Error al abrir archivos");
        return;
    }

    size_t key_len = strlen(key);
    int i = 0;
    char c;

    while ((c = fgetc(input)) != EOF) {
        char k = key[i % key_len];
        char new_c = encrypt_char(c, k, 0);
        fputc(new_c, output);
        i++;
    }

    fclose(input);
    fclose(output);
    printf("Archivo encriptado correctamente: %s\n", output_path);
}

// Desencripta (igual que cifrar pero invierte el desplazamiento)
void vigenere_decrypt(const char *input_path, const char *output_path, const char *key) {
    FILE *input = fopen(input_path, "r");
    FILE *output = fopen(output_path, "w");

    if (!input || !output) {
        perror("Error al abrir archivos");
        return;
    }

    size_t key_len = strlen(key);
    int i = 0;
    char c;

    while ((c = fgetc(input)) != EOF) {
        char k = key[i % key_len];
        char new_c = encrypt_char(c, k, 1); // 1 = modo desencriptar
        fputc(new_c, output);
        i++;
    }

    fclose(input);
    fclose(output);
    printf("Archivo desencriptado correctamente: %s\n", output_path);
}
