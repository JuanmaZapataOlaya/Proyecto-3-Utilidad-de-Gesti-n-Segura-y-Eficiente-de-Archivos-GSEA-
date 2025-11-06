# Variables
CC = gcc
CFLAGS = -Wall -Iinclude -std=c99
LDFLAGS = -pthread
BUILD_DIR = build
SRC_DIR = src
BIN_NAME = gestor

# Archivos fuente C (todos los .c)
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Archivos objeto correspondientes (en el directorio build)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean

# Objetivo principal: compilar el ejecutable
all: $(BUILD_DIR) $(BUILD_DIR)/$(BIN_NAME)

# Crear el directorio de construcción si no existe
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Enlace: Crear el ejecutable final
$(BUILD_DIR)/$(BIN_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compilación de módulos (.o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -rf $(BUILD_DIR)
