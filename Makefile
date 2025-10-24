CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/main.c src/encryption.c
OUT = build/gsea

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run:
	./$(OUT)

clean:
	rm -f $(OUT)