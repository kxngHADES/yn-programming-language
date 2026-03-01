#include "compiler.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void Compiler_compile(Compiler* self) {
    FILE* in = fopen(self->input_file, "r");
    if (!in) {
        printf("Error: Could not open file %s\n", self->input_file);
        exit(1);
    }

    FILE* out = fopen(self->c_file, "w");
    if (!out) {
        printf("Error: Could not create standard C file %s\n", self->c_file);
        fclose(in);
        exit(1);
    }

    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdbool.h>\n");
    fprintf(out, "\n");
    fprintf(out, "int main() {\n");

    SymbolTable* st = create_symbol_table();
    Parser* parser = create_parser(in, out, st);

    parser->parse_file(parser);

    int has_error = parser->has_error;

    destroy_parser(parser);
    destroy_symbol_table(st);

    if (has_error) {
        fclose(in);
        fclose(out);
        remove(self->c_file);
        printf("\nCompilation aborted due to syntax errors.\n");
        exit(1);
    }

    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");

    fclose(in);
    fclose(out);

    // Call GCC
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "gcc %s -o %s", self->c_file, self->exe_file);
    
    int ret = system(cmd);
    
    if (ret == 0) {
        char run_cmd[512];
#ifdef _WIN32
        snprintf(run_cmd, sizeof(run_cmd), "%s", self->exe_file);
#else
        snprintf(run_cmd, sizeof(run_cmd), "./%s", self->exe_file);
#endif
        system(run_cmd);

        remove(self->c_file);
        remove(self->exe_file);
    } else {
        printf("Compilation failed during gcc stage.\n");
    }
}

Compiler* create_compiler(const char* input_file) {
    Compiler* comp = (Compiler*)malloc(sizeof(Compiler));
    strcpy(comp->input_file, input_file);

    strcpy(comp->c_file, "temp_out.c");
    strcpy(comp->exe_file, "a.exe");

    const char *dot = strrchr(input_file, '.');
    const char *slash = strrchr(input_file, '\\');
    const char *fslash = strrchr(input_file, '/');
    const char *basename = input_file;

    if (slash && slash > basename) basename = slash + 1;
    if (fslash && fslash > basename) basename = fslash + 1;

    if (dot && dot > basename) {
        int len = dot - basename;
        strncpy(comp->c_file, basename, len);
        comp->c_file[len] = '\0';
        strcpy(comp->exe_file, comp->c_file);
        strcat(comp->c_file, ".c");
        
#ifdef _WIN32
        strcat(comp->exe_file, ".exe");
#endif
    }
    comp->compile = Compiler_compile;
    return comp;
}

void destroy_compiler(Compiler* comp) {
    if (comp) free(comp);
}
