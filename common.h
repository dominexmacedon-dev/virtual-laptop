#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_NULL
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        float f;
        int b;
    } as;
} Value;

typedef enum {

    OP_CONSTANT,

    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,

    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,

    OP_NOT,
    OP_AND,
    OP_OR,

    OP_POP,
    OP_DUP,

    OP_OUT,

    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,

    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,

    OP_CALL,
    OP_RETURN,

    OP_END

} OpCode;

#endif