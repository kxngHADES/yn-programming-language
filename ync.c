#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define YN_VERSION "1.0.1"

// ==========================================
// Application Entry Point
// ==========================================
int main(int argc, char** argv) {
    if (argc >= 2) {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            printf("YN Language Compiler (ync) v%s - OOP Native Engine\n", YN_VERSION);
            return 0;
        }
    }

    if (argc < 2) {
        printf("YN Language Compiler (ync) v%s - OOP Native Engine\n", YN_VERSION);
        printf("Usage: yn <script.yn>\n");
        printf("       yn --version\n");
        return 1;
    }
    
    Compiler* comp = create_compiler(argv[1]);
    comp->compile(comp);
    destroy_compiler(comp);
    return 0;
}
