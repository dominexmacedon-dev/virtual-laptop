#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chunk.h"
#include "vm.h"
#include "compiler.h"

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", path);
        exit(1);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read file: %s\n", path);
        exit(1);
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static bool hasExtension(const char* path, const char* ext) {
    size_t pathLen = strlen(path);
    size_t extLen = strlen(ext);

    if (pathLen < extLen) return false;

    return strcmp(path + pathLen - extLen, ext) == 0;
}

static void runFile(const char* path) {
    if (!hasExtension(path, ".plusa")) {
        fprintf(stderr, "Error: only .plusa files are supported\n");
        exit(1);
    }

    char* source = readFile(path);

    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        fprintf(stderr, "Compilation failed\n");
        freeChunk(&chunk);
        free(source);
        exit(1);
    }

    InterpretResult result = interpret(&chunk);

    if (result == INTERPRET_COMPILE_ERROR) {
        fprintf(stderr, "Compile error\n");
    } else if (result == INTERPRET_RUNTIME_ERROR) {
        fprintf(stderr, "Runtime error\n");
    }

    freeChunk(&chunk);
    free(source);
}

static void repl(void) {
    char line[1024];

    Chunk chunk;
    initChunk(&chunk);

    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) break;

        if (strcmp(line, "exit\n") == 0) break;

        freeChunk(&chunk);
        initChunk(&chunk);

        if (!compile(line, &chunk)) {
            fprintf(stderr, "Compilation error\n");
            continue;
        }

        interpret(&chunk);
    }

    freeChunk(&chunk);
}

int main(int argc, const char* argv[]) {
    initVM();

    if (argc == 2) {
        runFile(argv[1]);
    } else {
        repl();
    }

    freeVM();
    return 0;
}
