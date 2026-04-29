#include "chunk.h"
#include <stdlib.h>

static void* reallocate(void* ptr, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, newSize);
}

static int grow(int capacity) {
    return capacity < 8 ? 8 : capacity * 2;
}

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    chunk->lines = NULL;

    chunk->constantsCount = 0;
    chunk->constantsCapacity = 0;
    chunk->constants = NULL;
}

void freeChunk(Chunk* chunk) {
    free(chunk->code);
    free(chunk->lines);
    free(chunk->constants);

    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    chunk->lines = NULL;

    chunk->constantsCount = 0;
    chunk->constantsCapacity = 0;
    chunk->constants = NULL;
}

void writeChunk(Chunk* chunk, uint8_t byte) {
    if (chunk->count + 1 > chunk->capacity) {
        int old = chunk->capacity;
        chunk->capacity = grow(old);

        chunk->code = (uint8_t*)reallocate(
            chunk->code,
            old * sizeof(uint8_t),
            chunk->capacity * sizeof(uint8_t)
        );

        chunk->lines = (int*)reallocate(
            chunk->lines,
            old * sizeof(int),
            chunk->capacity * sizeof(int)
        );
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = 0;
    chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
    if (chunk->constantsCount + 1 > chunk->constantsCapacity) {
        int old = chunk->constantsCapacity;
        chunk->constantsCapacity = grow(old);

        chunk->constants = (Value*)reallocate(
            chunk->constants,
            old * sizeof(Value),
            chunk->constantsCapacity * sizeof(Value)
        );
    }

    chunk->constants[chunk->constantsCount] = value;
    return chunk->constantsCount++;
}

Value intVal(int v) {
    Value val;
    val.type = VAL_INT;
    val.as.i = v;
    return val;
}

Value floatVal(float v) {
    Value val;
    val.type = VAL_FLOAT;
    val.as.f = v;
    return val;
}

Value boolVal(int v) {
    Value val;
    val.type = VAL_BOOL;
    val.as.b = v;
    return val;
}