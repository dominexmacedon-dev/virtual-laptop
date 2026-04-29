#ifndef VM_H
#define VM_H

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;

    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);

void interpret(VM* vm, Chunk* chunk);

#endif