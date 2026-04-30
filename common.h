#ifndef common_h
#define common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_TRACE_EXECUTION 0

typedef double Value;

typedef enum {

    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,

    OP_POP,
    OP_DUP,

    OP_GET_LOCAL,
    OP_SET_LOCAL,

    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,

    OP_GET_UPVALUE,
    OP_SET_UPVALUE,

    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,

    OP_EQUAL,
    OP_GREATER,
    OP_LESS,

    OP_NOT,

    OP_AND,
    OP_OR,

    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,

    OP_CALL,
    OP_RETURN,

    OP_CLOSURE,
    OP_CLOSE_UPVALUE,

    OP_CLASS,
    OP_METHOD,
    OP_INVOKE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,

    OP_PRINT,

    OP_END

} OpCode;

#define UINT8_COUNT (UINT8_MAX + 1)

#define ALLOCATE(type, count) \
    (type*)realloc(NULL, sizeof(type) * (count))

#define FREE(ptr) free(ptr)

#if DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#endif
