#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"
#include "common.h"

typedef struct {
    const char* start;
    const char* current;
    int line;
} Lexer;

typedef enum {
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

static Lexer lexer;
static Compiler* currentCompiler;

static void initLexer(const char* source) {
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static bool isAtEnd(void) {
    return *lexer.current == '\0';
}

static char advanceChar(void) {
    lexer.current++;
    return lexer.current[-1];
}

static char peek(void) {
    return *lexer.current;
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    token.line = lexer.line;
    return token;
}

static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer.line;
    return token;
}

static void skipWhitespace(void) {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advanceChar();
                break;
            case '\n':
                lexer.line++;
                advanceChar();
                break;
            case '/':
                if (lexer.current[1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) advanceChar();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static Token number(void) {
    while (peek() >= '0' && peek() <= '9') advanceChar();
    return makeToken(TOKEN_NUMBER);
}

static Token identifier(void) {
    while ((peek() >= 'a' && peek() <= 'z') ||
           (peek() >= 'A' && peek() <= 'Z') ||
           (peek() >= '0' && peek() <= '9') ||
           peek() == '_') {
        advanceChar();
    }
    return makeToken(TOKEN_IDENTIFIER);
}

static Token scanToken(void) {
    skipWhitespace();

    lexer.start = lexer.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advanceChar();

    if (c >= '0' && c <= '9') return number();
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') return identifier();

    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(advanceChar() == '=' ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(advanceChar() == '=' ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(advanceChar() == '=' ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(advanceChar() == '=' ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    }

    return errorToken("Unexpected character.");
}

static void emitByte(Chunk* chunk, uint8_t byte, int line) {
    writeChunk(chunk, byte, line);
}

static void emitBytes(Chunk* chunk, uint8_t a, uint8_t b, int line) {
    emitByte(chunk, a, line);
    emitByte(chunk, b, line);
}

static void emitConstant(Chunk* chunk, Value value, int line) {
    int constant = addConstant(chunk, value);
    emitByte(chunk, OP_CONSTANT, line);
    emitByte(chunk, (uint8_t)constant, line);
}

static void emitReturn(Chunk* chunk, int line) {
    emitByte(chunk, OP_RETURN, line);
}

static void compileExpression(Chunk* chunk) {
    Token token = scanToken();

    if (token.type == TOKEN_NUMBER) {
        double value = strtod(token.start, NULL);
        emitConstant(chunk, NUMBER_VAL(value), token.line);
    }

    Token next = scanToken();

    if (next.type == TOKEN_PLUS) {
        compileExpression(chunk);
        emitByte(chunk, OP_ADD, next.line);
    }

    if (next.type == TOKEN_MINUS) {
        compileExpression(chunk);
        emitByte(chunk, OP_SUBTRACT, next.line);
    }
}

bool compile(const char* source, Chunk* chunk) {
    initLexer(source);
    currentCompiler = (Compiler*)malloc(sizeof(Compiler));
    currentCompiler->chunk = chunk;

    compileExpression(chunk);

    emitReturn(chunk, lexer.line);

    free(currentCompiler);
    return true;
}
