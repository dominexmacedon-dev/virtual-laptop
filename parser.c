#include "parser.h"
#include <stdlib.h>

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
    if (parser->current.type == type) {
        advance(parser);
    }
}

static void emitByte(Parser* parser, uint8_t byte) {
    writeChunk(parser->chunk, byte);
}

static int makeConstant(Parser* parser, Value value) {
    return addConstant(parser->chunk, value);
}

static void expression(Parser* parser);

static void number(Parser* parser) {
    Value value = intVal(atoi(parser->previous.start));
    int constant = makeConstant(parser, value);
    emitByte(parser, OP_CONSTANT);
    emitByte(parser, constant);
}

static void grouping(Parser* parser) {
    expression(parser);
    consume(parser, TOKEN_RIGHT_PAREN);
}

static void unary(Parser* parser) {
    TokenType operatorType = parser->previous.type;

    expression(parser);

    if (operatorType == TOKEN_MINUS) {
        emitByte(parser, OP_NEGATE);
    }
}

static void factor(Parser* parser) {
    advance(parser);

    if (parser->previous.type == TOKEN_NUMBER) {
        number(parser);
        return;
    }

    if (parser->previous.type == TOKEN_LEFT_PAREN) {
        grouping(parser);
        return;
    }

    if (parser->previous.type == TOKEN_MINUS) {
        unary(parser);
        return;
    }
}

static void term(Parser* parser) {
    factor(parser);

    while (check(parser, TOKEN_STAR) || check(parser, TOKEN_SLASH)) {
        TokenType op = parser->current.type;
        advance(parser);
        factor(parser);

        if (op == TOKEN_STAR) emitByte(parser, OP_MULTIPLY);
        if (op == TOKEN_SLASH) emitByte(parser, OP_DIVIDE);
    }
}

static void expression(Parser* parser) {
    term(parser);

    while (check(parser, TOKEN_PLUS) || check(parser, TOKEN_MINUS)) {
        TokenType op = parser->current.type;
        advance(parser);
        term(parser);

        if (op == TOKEN_PLUS) emitByte(parser, OP_ADD);
        if (op == TOKEN_MINUS) emitByte(parser, OP_SUBTRACT);
    }
}

static void statement(Parser* parser) {
    if (match(parser, TOKEN_OUT)) {
        expression(parser);
        emitByte(parser, OP_OUT);
        consume(parser, TOKEN_SEMICOLON);
    } else {
        expression(parser);
        consume(parser, TOKEN_SEMICOLON);
        emitByte(parser, OP_POP);
    }
}

static void program(Parser* parser) {
    advance(parser);

    while (parser->current.type != TOKEN_EOF) {
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