#include "symbol_table.h"
#include <string.h>
#include <stdlib.h>

void SymbolTable_add(SymbolTable* self, const char* name, const char* type) {
    if (self->count >= MAX_VARS) return;
    strcpy(self->vars[self->count].name, name);
    strcpy(self->vars[self->count].type, type);
    self->count++;
}

const char* SymbolTable_get_type(SymbolTable* self, const char* name) {
    for (int i = 0; i < self->count; i++) {
        if (strcmp(self->vars[i].name, name) == 0) {
            return self->vars[i].type;
        }
    }
    return NULL; 
}

SymbolTable* create_symbol_table() {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
    st->count = 0;
    st->add = SymbolTable_add;
    st->get_type = SymbolTable_get_type;
    return st;
}

void destroy_symbol_table(SymbolTable* st) {
    if (st) free(st);
}
