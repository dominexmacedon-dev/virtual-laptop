#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define MAX_GLOBALS 256

typedef struct {
    const char* name;
    int length;
    int slot;
} Global;

static Global globals[MAX_GLOBALS];
static int globalCount = 0;

static int identifierConstant(Parser* parser, const char* name, int length) {
    for (int i = 0; i < globalCount; i++) {
        if (globals[i].length == length &&
            strncmp(globals[i].name, name, length) == 0) {
            return globals[i].slot;
        }
    }

    int slot = globalCount++;
    globals[slot].name = name;
    globals[slot].length = length;
    globals[slot].slot = slot;

    return slot;
}

static void advance(Parser* parser) {
    parser->previous = parser->current;
    parser->current = scanToken(&parser->lexer);
}

static int check(Parser* parser, TokenType type) {
    return parser->current.type == type;
}

static int match(Parser* parser, TokenType type) {
    if (!check(parser, type)) return 0;
    advance(parser);
    return 1;
}

static void consume(Parser* parser, TokenType type) {
    if (parser->current.type == type) advance(parser);
}

static void emitByte(Parser* parser, uint8_t byte) {
    writeChunk(parser->chunk, byte);
}

static int makeConstant(Parser* parser, Value value) {
    return addConstant(parser->chunk, value);
}

static void emitConstant(Parser* parser, Value value) {
    int constant = makeConstant(parser, value);
    emitByte(parser, OP_CONSTANT);
    emitByte(parser, constant);
}

static void expression(Parser* parser);

static void variableLoad(Parser* parser, Token nameToken) {
    int slot = identifierConstant(parser, nameToken.start, nameToken.length);
    emitByte(parser, OP_GET_GLOBAL);
    emitByte(parser, slot);
}

static void variableAssign(Parser* parser, Token nameToken) {
    int slot = identifierConstant(parser, nameToken.start, nameToken.length);
    expression(parser);
    emitByte(parser, OP_DEFINE_GLOBAL);
    emitByte(parser, slot);
}

static void primary(Parser* parser) {
    advance(parser);

    if (parser->previous.type == TOKEN_NUMBER) {
        emitConstant(parser, intVal(atoi(parser->previous.start)));
        return;
    }

    if (parser->previous.type == TOKEN_IDENTIFIER) {
        variableLoad(parser, parser->previous);
        return;
    }

    if (parser->previous.type == TOKEN_LEFT_PAREN) {
        expression(parser);
        consume(parser, TOKEN_RIGHT_PAREN);
        return;
    }
}

static void unary(Parser* parser) {
    if (match(parser, TOKEN_MINUS)) {
        unary(parser);
        emitByte(parser, OP_NEGATE);
    } else if (match(parser, TOKEN_BANG)) {
        unary(parser);
        emitByte(parser, OP_NOT);
    } else if (match(parser, TOKEN_TILDE)) {
        unary(parser);
        emitByte(parser, OP_BIT_NOT);
    } else {
        primary(parser);
    }
}

static void factor(Parser* parser) {
    unary(parser);

    while (check(parser, TOKEN_STAR) ||
           check(parser, TOKEN_SLASH) ||
           check(parser, TOKEN_PERCENT)) {

        TokenType op = parser->current.type;
        advance(parser);
        unary(parser);

        if (op == TOKEN_STAR) emitByte(parser, OP_MULTIPLY);
        if (op == TOKEN_SLASH) emitByte(parser, OP_DIVIDE);
        if (op == TOKEN_PERCENT) emitByte(parser, OP_MOD);
    }
}

static void term(Parser* parser) {
    factor(parser);

    while (check(parser, TOKEN_PLUS) ||
           check(parser, TOKEN_MINUS)) {

        TokenType op = parser->current.type;
        advance(parser);
        factor(parser);

        if (op == TOKEN_PLUS) emitByte(parser, OP_ADD);
        if (op == TOKEN_MINUS) emitByte(parser, OP_SUBTRACT);
    }
}

static void shift(Parser* parser) {
    term(parser);

    while (check(parser, TOKEN_SHIFT_LEFT) ||
           check(parser, TOKEN_SHIFT_RIGHT)) {

        TokenType op = parser->current.type;
        advance(parser);
        term(parser);

        if (op == TOKEN_SHIFT_LEFT) emitByte(parser, OP_SHIFT_LEFT);
        if (op == TOKEN_SHIFT_RIGHT) emitByte(parser, OP_SHIFT_RIGHT);
    }
}

static void comparison(Parser* parser) {
    shift(parser);

    while (check(parser, TOKEN_GREATER) ||
           check(parser, TOKEN_GREATER_EQUAL) ||
           check(parser, TOKEN_LESS) ||
           check(parser, TOKEN_LESS_EQUAL)) {

        TokenType op = parser->current.type;
        advance(parser);
        shift(parser);

        if (op == TOKEN_GREATER) emitByte(parser, OP_GREATER);
        if (op == TOKEN_GREATER_EQUAL) emitByte(parser, OP_GREATER_EQUAL);
        if (op == TOKEN_LESS) emitByte(parser, OP_LESS);
        if (op == TOKEN_LESS_EQUAL) emitByte(parser, OP_LESS_EQUAL);
    }
}

static void equality(Parser* parser) {
    comparison(parser);

    while (check(parser, TOKEN_EQUAL_EQUAL) ||
           check(parser, TOKEN_BANG_EQUAL)) {

        TokenType op = parser->current.type;
        advance(parser);
        comparison(parser);

        if (op == TOKEN_EQUAL_EQUAL) emitByte(parser, OP_EQUAL);
        if (op == TOKEN_BANG_EQUAL) emitByte(parser, OP_NOT_EQUAL);
    }
}

static void bitwise(Parser* parser) {
    equality(parser);

    while (check(parser, TOKEN_AMP) ||
           check(parser, TOKEN_PIPE) ||
           check(parser, TOKEN_CARET)) {

        TokenType op = parser->current.type;
        advance(parser);
        equality(parser);

        if (op == TOKEN_AMP) emitByte(parser, OP_BIT_AND);
        if (op == TOKEN_PIPE) emitByte(parser, OP_BIT_OR);
        if (op == TOKEN_CARET) emitByte(parser, OP_BIT_XOR);
    }
}

static void logic(Parser* parser) {
    bitwise(parser);

    while (check(parser, TOKEN_AND_AND) ||
           check(parser, TOKEN_OR_OR)) {

        TokenType op = parser->current.type;
        advance(parser);
        bitwise(parser);

        if (op == TOKEN_AND_AND) emitByte(parser, OP_AND);
        if (op == TOKEN_OR_OR) emitByte(parser, OP_OR);
    }
}

static void expression(Parser* parser) {
    logic(parser);
}

static void statement(Parser* parser) {

    if (match(parser, TOKEN_LET)) {
        Token nameToken = parser->current;
        consume(parser, TOKEN_IDENTIFIER);

        int slot = identifierConstant(parser, nameToken.start, nameToken.length);

        if (match(parser, TOKEN_EQUAL)) {
            expression(parser);
        } else {
            emitConstant(parser, intVal(0));
        }

        emitByte(parser, OP_DEFINE_GLOBAL);
        emitByte(parser, slot);

        consume(parser, TOKEN_SEMICOLON);
        return;
    }

    if (match(parser, TOKEN_OUT)) {
        expression(parser);
        emitByte(parser, OP_OUT);
        consume(parser, TOKEN_SEMICOLON);
        return;
    }

    if (parser->current.type == TOKEN_IDENTIFIER &&
        parser->lexer.current != parser->lexer.start) {

        Token nameToken = parser->current;
        advance(parser);

        if (match(parser, TOKEN_EQUAL)) {
            int slot = identifierConstant(parser, nameToken.start, nameToken.length);
            expression(parser);
            emitByte(parser, OP_DEFINE_GLOBAL);
            emitByte(parser, slot);
            consume(parser, TOKEN_SEMICOLON);
            return;
        }

        variableLoad(parser, nameToken);
        emitByte(parser, OP_POP);
        consume(parser, TOKEN_SEMICOLON);
        return;
    }

    expression(parser);
    consume(parser, TOKEN_SEMICOLON);
    emitByte(parser, OP_POP);
}

static void program(Parser* parser) {
    advance(parser);

    while (!check(parser, TOKEN_EOF)) {
        statement(parser);
    }

    emitByte(parser, OP_RETURN);
}

void initParser(Parser* parser, const char* source, Chunk* chunk) {
    initLexer(&parser->lexer, source);
    parser->chunk = chunk;

    parser->current.type = TOKEN_ERROR;
    parser->previous.type = TOKEN_ERROR;

    advance(parser);
}

void parse(Parser* parser) {
    program(parser);
}
