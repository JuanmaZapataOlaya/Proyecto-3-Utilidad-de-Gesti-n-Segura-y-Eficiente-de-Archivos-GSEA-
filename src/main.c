#define _POSIX_C_SOURCE 200809L 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 
#include <stdbool.h>
#include <errno.h>

#include "../include/executor.h" 
#include "../include/utils.h"


void print_usage(char *program_name) {
    printf("Uso: %s [-c|-e|-u|-d|-ce|-ec|-ud|-dc|...] -i <input> -o <output> [-k <clave>]\n", program_name);
    printf("Modos de operación:\n");
    printf("  -c : Comprimir (RLE)\n");
    printf("  -e : Encriptar (Vigenere)\n");
    printf("  -u : Desencriptar (Vigenere)\n");
    printf("  -d : Descomprimir (RLE)\n");
    printf("  -ce: Comprimir y Encriptar\n");
    printf("  -ud: Desencriptar y Descomprimir (Restauracion)\n");
    printf("  * Otros modos combinados funcionan automaticamente (ej: -ec, -dc, -eu, -cu, etc.)\n");
}


int main(int argc, char *argv[]) {
    // errno es para los errores
    extern int errno; 

    // Verifica la cantidad mínima de argumentos, al principio pusimos 7 pero eran muy pocos
    if (argc < 6) { 
        print_usage(argv[0]);
        return 1;
    }

    char *input_path = NULL;
    char *output_path = NULL;
    char *key = NULL;
    
    OperationType op_sequence[4] = {OP_NONE, OP_NONE, OP_NONE, OP_NONE}; 
    int op_index = 0; 

    // PARSEO DE ARGUMENTOS
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) { 
            input_path = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        }
        else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            key = argv[++i];
        }
        else if (argv[i][0] == '-' && argv[i][1] != '-' && strlen(argv[i]) >= 2 && op_index < 4) {
            for (size_t j = 1; j < strlen(argv[i]) && op_index < 4; j++) {
                switch (argv[i][j]) {
                    case 'c': op_sequence[op_index++] = OP_COMPRESS; break;
                    case 'e': op_sequence[op_index++] = OP_ENCRYPT; break;
                    case 'd': op_sequence[op_index++] = OP_DECOMPRESS; break;
                    case 'u': op_sequence[op_index++] = OP_DECRYPT; break;
                    default: 
                        printf("Error: Operacion desconocida -%c\n", argv[i][j]); 
                        return 1;
                }
            }
        }
    }
    
    // VERIFICACIONES Y VALIDACIÓN DE CLAVE
    if (!input_path || !output_path || op_index == 0) {
        printf("Error: faltan parametros obligatorios o no se especifico operacion.\n");
        print_usage(argv[0]);
        return 1;
    }
    
    bool key_required = false;
    for (int i = 0; i < op_index; i++) {
        if (op_sequence[i] == OP_ENCRYPT || op_sequence[i] == OP_DECRYPT) {
            key_required = true;
            break;
        }
    }
    if (key_required && !key) {
        printf("Error: La clave secreta (-k) es obligatoria para las operaciones de encriptacion/desencriptacion.\n");
        return 1;
    }


    if (is_directory(input_path)) {
        return process_directory_concurrently(input_path, output_path, op_sequence, key);
        
    } else {
        printf("Detectado archivo: %s. Iniciando procesamiento SECUENCIAL.\n", input_path);
        
        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("Error de malloc para argumentos secuenciales");
            return 1;
        }

        // Duplicamos las rutas a memoria dinámica, que será liberada dentro de process_file_pipeline
        args->input_file_path = strdup(input_path); 
        args->output_file_path = strdup(output_path);
        args->key = key; 
        memcpy(args->sequence, op_sequence, sizeof(OperationType) * 4);

        if (args->input_file_path == NULL || args->output_file_path == NULL) {
            perror("Error en strdup para argumentos secuenciales");
            if (args->input_file_path) free(args->input_file_path);
            if (args->output_file_path) free(args->output_file_path);
            free(args);
            return 1;
        }

        process_file_pipeline(args);

        printf("Procesamiento secuencial de archivo finalizado.\n");
    }

    return 0;
}