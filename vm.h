#ifndef VM_H
#define VM_H

#include "chunk.h"
#include "common.h"

#define STACK_MAX 256
#define GLOBAL_MAX 256
#define LOCAL_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;

    Value stack[STACK_MAX];
    Value* stackTop;

    Value globals[GLOBAL_MAX];
    Value locals[LOCAL_MAX];
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);

void interpret(VM* vm, Chunk* chunk);

#endif
