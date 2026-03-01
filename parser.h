#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include "symbol_table.h"

typedef struct Parser {
    SymbolTable* sym_table;
    int line_num;
    int has_error;
    FILE* in;
    FILE* out;

    // Methods
    void (*parse_line)(struct Parser* self, char* line);
    void (*parse_file)(struct Parser* self);
} Parser;

Parser* create_parser(FILE* in, FILE* out, SymbolTable* st);
void destroy_parser(Parser* p);

#endif // PARSER_H
