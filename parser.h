#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "chunk.h"

typedef struct {
    Lexer lexer;
    Token current;
    Token previous;
    Chunk* chunk;
} Parser;

void initParser(Parser* parser, const char* source, Chunk* chunk);
void parse(Parser* parser);

#endif