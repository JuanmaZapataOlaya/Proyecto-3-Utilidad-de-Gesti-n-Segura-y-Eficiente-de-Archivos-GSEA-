#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <pthread.h>
#include <stdbool.h>

// Enum para operaciones
typedef enum {
    OP_NONE = 0,
    OP_ENCRYPT,
    OP_DECRYPT,
    OP_COMPRESS,
    OP_DECOMPRESS
} OperationType;

// Estructura de argumentos
typedef struct {
    char *input_file_path;
    char *output_file_path;
    char *key;
    OperationType sequence[4];
} ThreadArgs;


// Declaraciones de funciones de ejecución que se usarán en main.c
void *process_file_pipeline(void *arg);
int process_directory_concurrently(char *input_dir, char *output_dir, OperationType *op_sequence, char *key);


#endif 