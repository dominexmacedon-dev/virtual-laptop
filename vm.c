#include <stdio.h>
#include "vm.h"
#include "common.h"

VM vm;

static void resetStack(void) {
    vm.stackTop = vm.stack;
}

void initVM(void) {
    resetStack();
    vm.chunk = NULL;
    vm.ip = NULL;
}

void freeVM(void) {
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(void) {
    vm.stackTop--;
    return *vm.stackTop;
}

static uint8_t readByte(void) {
    return *vm.ip++;
}

static uint16_t readShort(void) {
    vm.ip += 2;
    return (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]);
}

static Value readConstant(void) {
    return vm.chunk->constants.values[readByte()];
}

static void runtimeError(const char* message) {
    fprintf(stderr, "%s\n", message);
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static InterpretResult run(void) {
    for (;;) {
        uint8_t instruction = readByte();

        switch (instruction) {

            case OP_CONSTANT: {
                push(readConstant());
                break;
            }

            case OP_NIL: {
                push(NIL_VAL);
                break;
            }

            case OP_TRUE: {
                push(BOOL_VAL(true));
                break;
            }

            case OP_FALSE: {
                push(BOOL_VAL(false));
                break;
            }

            case OP_POP: {
                pop();
                break;
            }

            case OP_DUP: {
                push(vm.stackTop[-1]);
                break;
            }

            case OP_GET_LOCAL: {
                uint8_t slot = readByte();
                push(vm.stack[slot]);
                break;
            }

            case OP_SET_LOCAL: {
                uint8_t slot = readByte();
                vm.stack[slot] = vm.stackTop[-1];
                break;
            }

            case OP_GET_GLOBAL: {
                break;
            }

            case OP_DEFINE_GLOBAL: {
                pop();
                break;
            }

            case OP_SET_GLOBAL: {
                break;
            }

            case OP_GET_UPVALUE: {
                break;
            }

            case OP_SET_UPVALUE: {
                break;
            }

            case OP_ADD: {
                push(NUMBER_VAL(AS_NUMBER(pop()) + AS_NUMBER(pop())));
                break;
            }

            case OP_SUBTRACT: {
                Value b = pop();
                Value a = pop();
                push(NUMBER_VAL(AS_NUMBER(a) - AS_NUMBER(b)));
                break;
            }

            case OP_MULTIPLY: {
                push(NUMBER_VAL(AS_NUMBER(pop()) * AS_NUMBER(pop())));
                break;
            }

            case OP_DIVIDE: {
                Value b = pop();
                Value a = pop();
                push(NUMBER_VAL(AS_NUMBER(a) / AS_NUMBER(b)));
                break;
            }

            case OP_NEGATE: {
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }

            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }

            case OP_GREATER: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(AS_NUMBER(a) > AS_NUMBER(b)));
                break;
            }

            case OP_LESS: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(AS_NUMBER(a) < AS_NUMBER(b)));
                break;
            }

            case OP_NOT: {
                push(BOOL_VAL(isFalsey(pop())));
                break;
            }

            case OP_AND: {
                break;
            }

            case OP_OR: {
                break;
            }

            case OP_JUMP: {
                uint16_t offset = readShort();
                vm.ip += offset;
                break;
            }

            case OP_JUMP_IF_FALSE: {
                uint16_t offset = readShort();
                if (isFalsey(vm.stackTop[-1])) vm.ip += offset;
                break;
            }

            case OP_LOOP: {
                uint16_t offset = readShort();
                vm.ip -= offset;
                break;
            }

            case OP_CALL: {
                break;
            }

            case OP_RETURN: {
                return INTERPRET_OK;
            }

            case OP_CLOSURE: {
                break;
            }

            case OP_CLOSE_UPVALUE: {
                break;
            }

            case OP_CLASS: {
                break;
            }

            case OP_METHOD: {
                break;
            }

            case OP_INVOKE: {
                break;
            }

            case OP_GET_PROPERTY: {
                break;
            }

            case OP_SET_PROPERTY: {
                break;
            }

            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;
            }

            case OP_END: {
                return INTERPRET_OK;
            }
        }
    }
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = chunk->code;
    return run();
}
