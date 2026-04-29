#ifndef VALUE_H
#define VALUE_H

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

Value intVal(int v);
Value floatVal(float v);
Value boolVal(int v);
Value nullVal(void);

int isFalse(Value v);
int valuesEqual(Value a, Value b);

#endif