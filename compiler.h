#ifndef COMPILER_H
#define COMPILER_H

typedef struct Compiler {
    char input_file[256];
    char c_file[256];
    char exe_file[256];

    // Methods
    void (*compile)(struct Compiler* self);
} Compiler;

Compiler* create_compiler(const char* input_file);
void destroy_compiler(Compiler* comp);

#endif // COMPILER_H
