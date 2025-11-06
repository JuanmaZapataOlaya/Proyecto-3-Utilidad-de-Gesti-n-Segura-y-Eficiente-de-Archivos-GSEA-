#define _POSIX_C_SOURCE 200809L // CRÍTICO: Habilita la definición de POSIX como strdup y mkstemp

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h> 
#include <pthread.h> 
#include <dirent.h> 
#include <unistd.h> // Para unlink(), close()
#include <errno.h>

#include "../include/executor.h" 
#include "../include/utils.h"    
#include "../include/encryption.h" 
#include "../include/compress.h" 


void *process_file_pipeline(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    
    char *current_input = args->input_file_path;
    char *next_output = NULL;

    printf("[Worker] Iniciando procesamiento de: %s\n", args->input_file_path);

    // Itera sobre la secuencia de operaciones
    for (int i = 0; i < 4 && args->sequence[i] != OP_NONE; i++) {
        OperationType op = args->sequence[i];
        
        // Determinar el archivo de salida (Temporal o Final)
        if (args->sequence[i+1] != OP_NONE) {
            // Si hay otra operación, el output es un archivo temporal
        
            char template[] = "/tmp/gsea_XXXXXX"; 
            int fd = mkstemp(template); 
            
            if (fd == -1) {
                perror("Error creando archivo temporal seguro (mkstemp)");
                // Liberar recursos 
                if (args->input_file_path) free(args->input_file_path);
                if (args->output_file_path) free(args->output_file_path);
                free(args);
                return NULL;
            }
            close(fd); 

            next_output = strdup(template);
            if (next_output == NULL) {
                perror("Error en strdup para archivo temporal");
                if (args->input_file_path) free(args->input_file_path);
                if (args->output_file_path) free(args->output_file_path);
                free(args);
                return NULL;
            }
        } else {
            next_output = args->output_file_path;
        }

        // Llama al algoritmo
        if (op == OP_COMPRESS) {
            printf("[Worker] -> Compresión: %s a %s\n", current_input, next_output);
            rle_compress(current_input, next_output); 
        } else if (op == OP_ENCRYPT) {
            printf("[Worker] -> Encriptación: %s a %s\n", current_input, next_output);
            vigenere_encrypt(current_input, next_output, args->key); 
        } else if (op == OP_DECOMPRESS) {
            printf("[Worker] -> Descompresión: %s a %s\n", current_input, next_output);
            rle_decompress(current_input, next_output); 
        } else if (op == OP_DECRYPT) {
            printf("[Worker] -> Desencriptación: %s a %s\n", current_input, next_output);
            vigenere_decrypt(current_input, next_output, args->key); 
        }

        // Gestión del archivo temporal y limpieza
        if (i > 0 && current_input != args->input_file_path) {
            if (unlink(current_input) != 0) {
                perror("Error al eliminar archivo temporal");
            }
            free(current_input); 
        }
        
        if (args->sequence[i+1] != OP_NONE) {
            current_input = next_output;
        } else {
            printf("Archivo %s correctamente.\n", (op == OP_ENCRYPT || op == OP_COMPRESS) ? "procesado" : "restaurado");
        }
    }
    
    // Las rutas originales se liberan aquí.
    if (args->input_file_path) free(args->input_file_path);
    if (args->output_file_path) free(args->output_file_path);
    free(args);

    return NULL;
}


int process_directory_concurrently(char *input_dir, char *output_dir, OperationType *op_sequence, char *key) {
    DIR *dir;
    struct dirent *entry;
    pthread_t *threads = NULL;
    int thread_count = 0;

    dir = opendir(input_dir);
    if (dir == NULL) {
        perror("Error: No se pudo abrir el directorio de entrada");
        return 1;
    }
    
    if (mkdir(output_dir, 0777) != 0 && errno != EEXIST) {
        perror("Error al crear el directorio de salida");
        closedir(dir);
        return 1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *full_input = build_path(input_dir, entry->d_name);
        
        if (is_directory(full_input)) { 
            free(full_input); 
            continue;
        }

        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (args == NULL) { 
            perror("Error de malloc para ThreadArgs"); 
            free(full_input); break; 
        }
        
        args->input_file_path = full_input; 
        args->output_file_path = build_path(output_dir, entry->d_name);
        args->key = key; 
        memcpy(args->sequence, op_sequence, sizeof(OperationType) * 4);
        
        //Crear el hilo
        threads = (pthread_t *)realloc(threads, (thread_count + 1) * sizeof(pthread_t));
        if (threads == NULL) {
            perror("Error de realloc para el array de hilos");
            // Liberación de recursos en caso de fallo de realloc
            if (args->output_file_path) free(args->output_file_path); 
            if (args->input_file_path) free(args->input_file_path); 
            free(args); 
            break;
        }

        if (pthread_create(&threads[thread_count], NULL, process_file_pipeline, args) != 0) {
            perror("Error al crear el hilo");
        } else {
            thread_count++;
        }
    }
    
    closedir(dir);

    //Esperar a que todos terminen
    printf("Esperando a que %d hilos terminen...\n", thread_count);
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    if (threads) free(threads);
    return 0;
}