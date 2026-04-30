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
    OP_MOD,
    OP_POW,
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

    OP_BIT_AND,
    OP_BIT_OR,
    OP_BIT_XOR,
    OP_BIT_NOT,
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,

    OP_POP,
    OP_DUP,
    OP_SWAP,

    OP_OUT,

    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,

    OP_GET_LOCAL,
    OP_SET_LOCAL,

    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_JUMP_IF_TRUE,
    OP_LOOP,

    OP_CALL,
    OP_RETURN,

    OP_NOP,
    OP_END

} OpCode;

#endif
