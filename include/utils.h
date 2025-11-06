#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// 1. Verificación del Directorio
bool is_directory(const char *path);

// 2. Función Constructora de Rutas
char *build_path(const char *dir_path, const char *file_name);

// 3. Función auxiliar para crear un archivo temporal seguro
// Esta función ahora será utilizada internamente por el pipeline.
char *create_temp_file();

#endif 