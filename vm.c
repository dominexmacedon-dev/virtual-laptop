#include "vm.h"
#include <stdio.h>

static void resetStack(VM* vm) {
    vm->stackTop = vm->stack;
}

void initVM(VM* vm) {
    vm->chunk = NULL;
    vm->ip = NULL;
    resetStack(vm);
}

void freeVM(VM* vm) {
    (void)vm;
}

static void push(VM* vm, Value v) {
    *vm->stackTop = v;
    vm->stackTop++;
}

static Value pop(VM* vm) {
    vm->stackTop--;
    return *vm->stackTop;
}

static int isFalse(Value v) {
    if (v.type == VAL_BOOL) return v.as.b == 0;
    if (v.type == VAL_INT) return v.as.i == 0;
    return 0;
}

static int isEqual(Value a, Value b) {
    if (a.type != b.type) return 0;
    if (a.type == VAL_INT) return a.as.i == b.as.i;
    if (a.type == VAL_FLOAT) return a.as.f == b.as.f;
    if (a.type == VAL_BOOL) return a.as.b == b.as.b;
    return 0;
}

void interpret(VM* vm, Chunk* chunk) {
    vm->chunk = chunk;
    vm->ip = chunk->code;

    for (;;) {

        uint8_t instruction = *vm->ip++;

        switch (instruction) {

            case OP_CONSTANT: {
                uint8_t index = *vm->ip++;
                push(vm, chunk->constants[index]);
                break;
            }

            case OP_ADD: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, intVal(a.as.i + b.as.i));
                break;
            }

            case OP_SUBTRACT: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, intVal(a.as.i - b.as.i));
                break;
            }

            case OP_MULTIPLY: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, intVal(a.as.i * b.as.i));
                break;
            }

            case OP_DIVIDE: {
                Value b = pop(vm);
                Value a = pop(vm);
                if (b.as.i == 0) return;
                push(vm, intVal(a.as.i / b.as.i));
                break;
            }

            case OP_NEGATE: {
                Value v = pop(vm);
                push(vm, intVal(-v.as.i));
                break;
            }

            case OP_EQUAL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(isEqual(a, b)));
                break;
            }

            case OP_NOT_EQUAL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(!isEqual(a, b)));
                break;
            }

            case OP_GREATER: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(a.as.i > b.as.i));
                break;
            }

            case OP_GREATER_EQUAL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(a.as.i >= b.as.i));
                break;
            }

            case OP_LESS: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(a.as.i < b.as.i));
                break;
            }

            case OP_LESS_EQUAL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(a.as.i <= b.as.i));
                break;
            }

            case OP_NOT: {
                Value v = pop(vm);
                push(vm, boolVal(isFalse(v)));
                break;
            }

            case OP_AND: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(!isFalse(a) && !isFalse(b)));
                break;
            }

            case OP_OR: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, boolVal(!isFalse(a) || !isFalse(b)));
                break;
            }

            case OP_POP: {
                pop(vm);
                break;
            }

            case OP_DUP: {
                Value v = pop(vm);
                push(vm, v);
                push(vm, v);
                break;
            }

            case OP_OUT: {
                Value v = pop(vm);
                if (v.type == VAL_BOOL) {
                    printf("%s\n", v.as.b ? "true" : "false");
                } else if (v.type == VAL_FLOAT) {
                    printf("%f\n", v.as.f);
                } else {
                    printf("%d\n", v.as.i);
                }
                break;
            }

            case OP_JUMP: {
                uint16_t offset = (vm->ip[0] << 8) | vm->ip[1];
                vm->ip += 2 + offset;
                break;
            }

            case OP_JUMP_IF_FALSE: {
                uint16_t offset = (vm->ip[0] << 8) | vm->ip[1];
                vm->ip += 2;
                Value v = pop(vm);
                if (isFalse(v)) vm->ip += offset;
                break;
            }

            case OP_LOOP: {
                uint16_t offset = (vm->ip[0] << 8) | vm->ip[1];
                vm->ip -= offset;
                break;
            }

            case OP_DEFINE_GLOBAL:
            case OP_GET_GLOBAL:
            case OP_SET_GLOBAL: {
                return;
            }

            case OP_CALL: {
                return;
            }

            case OP_RETURN:
            case OP_END:
                return;

            default:
                return;
        }
    }
}