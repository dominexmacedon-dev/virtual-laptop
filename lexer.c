#include "lexer.h"
#include <string.h>
#include <ctype.h>

static int isAtEnd(Lexer* lexer) {
    return *lexer->current == '\0';
}

static char advance(Lexer* lexer) {
    lexer->current++;
    return lexer->current[-1];
}

static char peek(Lexer* lexer) {
    return *lexer->current;
}

static char peekNext(Lexer* lexer) {
    if (isAtEnd(lexer)) return '\0';
    return lexer->current[1];
}

static int match(Lexer* lexer, char expected) {
    if (isAtEnd(lexer)) return 0;
    if (*lexer->current != expected) return 0;
    lexer->current++;
    return 1;
}

static Token makeToken(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    return token;
}

static Token errorToken(Lexer* lexer, const char* msg) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (int)strlen(msg);
    token.line = lexer->line;
    return token;
}

static void skipWhitespace(Lexer* lexer) {
    for (;;) {
        char c = peek(lexer);

        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;

            case '\n':
                lexer->line++;
                advance(lexer);
                break;

            default:
                return;
        }
    }
}

static Token number(Lexer* lexer) {
    while (isdigit(peek(lexer))) advance(lexer);

    if (peek(lexer) == '.' && isdigit(peekNext(lexer))) {
        advance(lexer);
        while (isdigit(peek(lexer))) advance(lexer);
    }

    return makeToken(lexer, TOKEN_NUMBER);
}

static Token identifier(Lexer* lexer) {
    while (isalnum(peek(lexer)) || peek(lexer) == '_') advance(lexer);

    int len = (int)(lexer->current - lexer->start);

    if (len == 3 && strncmp(lexer->start, "out", 3) == 0)
        return makeToken(lexer, TOKEN_OUT);

    return makeToken(lexer, TOKEN_IDENTIFIER);
}

Token scanToken(Lexer* lexer) {
    skipWhitespace(lexer);

    lexer->start = lexer->current;

    if (isAtEnd(lexer))
        return makeToken(lexer, TOKEN_EOF);

    char c = advance(lexer);

    if (isdigit(c))
        return number(lexer);

    if (isalpha(c) || c == '_')
        return identifier(lexer);

    switch (c) {

        case '+': return makeToken(lexer, TOKEN_PLUS);
        case '-': return makeToken(lexer, TOKEN_MINUS);
        case '*': return makeToken(lexer, TOKEN_STAR);
        case '/': return makeToken(lexer, TOKEN_SLASH);
        case '%': return makeToken(lexer, TOKEN_PERCENT);
        case '^': return makeToken(lexer, TOKEN_CARET);

        case '&':
            if (match(lexer, '&')) return makeToken(lexer, TOKEN_AND_AND);
            return makeToken(lexer, TOKEN_AMP);

        case '|':
            if (match(lexer, '|')) return makeToken(lexer, TOKEN_OR_OR);
            return makeToken(lexer, TOKEN_PIPE);

        case '~': return makeToken(lexer, TOKEN_TILDE);

        case '<':
            if (match(lexer, '<')) return makeToken(lexer, TOKEN_SHIFT_LEFT);
            return makeToken(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);

        case '>':
            if (match(lexer, '>')) return makeToken(lexer, TOKEN_SHIFT_RIGHT);
            return makeToken(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

        case '=':
            return makeToken(lexer, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);

        case '!':
            return makeToken(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);

        case ';': return makeToken(lexer, TOKEN_SEMICOLON);

        case '(': return makeToken(lexer, TOKEN_LEFT_PAREN);
        case ')': return makeToken(lexer, TOKEN_RIGHT_PAREN);
    }

    return errorToken(lexer, "Unexpected character");
}

void initLexer(Lexer* lexer, const char* source) {
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
}
