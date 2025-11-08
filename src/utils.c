#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "../include/utils.h"

// Verificación del Directorio 
bool is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return false; 
    }
    return S_ISDIR(statbuf.st_mode);
}

// Función Constructora de Rutas 
char *build_path(const char *dir_path, const char *file_name) {
    // +2 para el separador '/' y el terminador nulo '\0'
    size_t len = strlen(dir_path) + strlen(file_name) + 2; 
    char *full_path = (char *)malloc(len);
    if (full_path == NULL) {
        perror("Fallo en la asignación de memoria para build_path");
        return NULL;
    }
    snprintf(full_path, len, "%s/%s", dir_path, file_name);
    return full_path;
}