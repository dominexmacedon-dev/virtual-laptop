#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "vm.h"
#include "parser.h"

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");

    if (!file) {
        printf("Could not open file: %s\n", path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);

    if (!buffer) {
        printf("Memory error\n");
        exit(1);
    }

    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: lang <file.lang>\n");
        return 1;
    }

    char* source = readFile(argv[1]);

    Chunk chunk;
    initChunk(&chunk);

    Parser parser;
    initParser(&parser, source, &chunk);
    parse(&parser);

    VM vm;
    initVM(&vm);
    interpret(&vm, &chunk);

    freeVM(&vm);
    freeChunk(&chunk);
    free(source);

    return 0;
}