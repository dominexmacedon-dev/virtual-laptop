#ifndef compiler_h
#define compiler_h

#include "chunk.h"

typedef struct {
    Chunk* chunk;
    int current;
    int line;
} Compiler;

bool compile(const char* source, Chunk* chunk);

#endif
