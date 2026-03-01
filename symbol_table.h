#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_VARS 500

typedef struct {
    char name[100];
    char type[50];
} Variable;

typedef struct SymbolTable {
    Variable vars[MAX_VARS];
    int count;

    // Methods
    void (*add)(struct SymbolTable* self, const char* name, const char* type);
    const char* (*get_type)(struct SymbolTable* self, const char* name);
} SymbolTable;

SymbolTable* create_symbol_table();
void destroy_symbol_table(SymbolTable* st);

#endif // SYMBOL_TABLE_H
