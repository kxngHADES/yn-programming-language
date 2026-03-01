#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Simple Symbol Table struct to keep track of variable names and types natively
#define MAX_VARS 100

typedef struct {
    char name[100];
    char type[50];
} Variable;

Variable sym_table[MAX_VARS];
int var_count = 0;

// Helper to look up var type by name
const char* get_var_type(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(sym_table[i].name, name) == 0) {
            return sym_table[i].type;
        }
    }
    return NULL; 
}

// Map from our custom "hold.this." types to C format types
const char* get_c_format(const char* var_type) {
    if (!var_type) return "%s"; // fallback
    if (strcmp(var_type, "hold.this.character") == 0) return "%c";
    if (strcmp(var_type, "hold.this.sting") == 0 || strcmp(var_type, "hold.this.string") == 0) return "%s";
    if (strcmp(var_type, "hold.this.int") == 0) return "%lld";
    if (strcmp(var_type, "hold.this.boolean") == 0) return "%s"; // Bools are tricky in C printf, we'll cast them to string or int in parser for now
    if (strcmp(var_type, "hold.this.float") == 0) return "%f";
    return "%s";
}

void trim_trailing_spaces(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void compile_file(const char* input_file) {
    char c_file[256];
    char exe_file[256];
    
    // Default names
    strcpy(c_file, "temp_out.c");
    strcpy(exe_file, "a.exe");

    // Try to derive output executable name from input file
    const char *dot = strrchr(input_file, '.');
    const char *slash = strrchr(input_file, '\\');
    const char *fslash = strrchr(input_file, '/');
    const char *basename = input_file;

    if (slash && slash > basename) basename = slash + 1;
    if (fslash && fslash > basename) basename = fslash + 1;

    if (dot && dot > basename) {
        int len = dot - basename;
        strncpy(c_file, basename, len);
        c_file[len] = '\0';
        strcpy(exe_file, c_file);
        strcat(c_file, ".c");
        
#ifdef _WIN32
        strcat(exe_file, ".exe");
#endif
    }

    FILE* in = fopen(input_file, "r");
    if (!in) {
        printf("Error: Could not open file %s\n", input_file);
        exit(1);
    }

    FILE* out = fopen(c_file, "w");
    if (!out) {
        printf("Error: Could not create standard C file %s\n", c_file);
        fclose(in);
        exit(1);
    }

    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdbool.h>\n");
    fprintf(out, "\n");
    fprintf(out, "int main() {\n");

    char line[1024];
    int line_num = 0;
    int has_error = 0;

    while (fgets(line, sizeof(line), in)) {
        line_num++;
        
        // Skip leading whitespace cleanly
        char *ptr = line;
        while(isspace((unsigned char)*ptr)) ptr++;
        if (*ptr == '\0' || strncmp(ptr, "//", 2) == 0) continue; // blank line or comment

        // ==========================
        // 1. Variable Declarations
        // ==========================
        if (strncmp(ptr, "hold.this.", 10) == 0) {
            char var_type[50];
            char var_name[100];
            char var_val[512];
            
            // extract parts safely
            int matched = sscanf(ptr, "%49s %99s = %511[^;];", var_type, var_name, var_val);
            if (matched == 3) {
                // Determine C equivalents
                if (strcmp(var_type, "hold.this.character") == 0) {
                    fprintf(out, "    char %s = %s;\n", var_name, var_val);
                } else if (strcmp(var_type, "hold.this.sting") == 0 || strcmp(var_type, "hold.this.string") == 0) {
                    fprintf(out, "    const char* %s = %s;\n", var_name, var_val);
                } else if (strcmp(var_type, "hold.this.int") == 0) {
                    fprintf(out, "    long long %s = %s;\n", var_name, var_val);
                } else if (strcmp(var_type, "hold.this.boolean") == 0) {
                    fprintf(out, "    bool %s = %s;\n", var_name, var_val);
                } else if (strcmp(var_type, "hold.this.float") == 0) {
                    fprintf(out, "    float %s = %s;\n", var_name, var_val);
                } else {
                    printf("Syntax Error on line %d: Unknown type '%s'\n", line_num, var_type);
                    has_error = 1;
                }

                // Register to symbol table
                if (!has_error) {
                    strcpy(sym_table[var_count].name, var_name);
                    strcpy(sym_table[var_count].type, var_type);
                    var_count++;
                }

            } else {
                 printf("Syntax Error on line %d: Variable declaration is malformed.\n", line_num);
                 has_error = 1;
            }
            continue;
        }

        // ==========================
        // 2. hear_my_mans Print Statement
        // ==========================
        if (strncmp(ptr, "hear_my_mans(", 13) == 0) {
            
            // Handle `f"..."` format strings securely
            if (strncmp(ptr, "hear_my_mans(f\"", 15) == 0) {
                char *str_start = ptr + 15;
                char *end = strstr(str_start, "\");");
                
                if (end) {
                    *end = '\0';
                    trim_trailing_spaces(str_start);
                    
                    char translated_str[1024] = "";
                    char args_str[1024] = "";
                    int is_first_arg = 1;
                    
                    char *c = str_start;
                    while (*c != '\0') {
                        if (*c == '{') {
                            c++; // skip {
                            char varname[100];
                            char format_spec[50] = "";
                            int i = 0;
                            int has_format = 0;
                            
                            while (*c != '}' && *c != '\0') {
                                if (*c == ':') {
                                    has_format = 1;
                                    c++;
                                    int f = 0;
                                    while (*c != '}' && *c != '\0') {
                                        format_spec[f++] = *c++;
                                    }
                                    format_spec[f] = '\0';
                                    break;
                                } else {
                                    varname[i++] = *c++;
                                }
                            }
                            varname[i] = '\0';
                            
                            if (*c == '}') {
                                c++; // skip }
                                const char *vt = get_var_type(varname);
                                if (!vt) {
                                    printf("Syntax Error on line %d: Variable '%s' is not defined!\n", line_num, varname);
                                    has_error = 1;
                                    break;
                                }

                                if (has_format) {
                                    // Manually construct the C printf format if user provided one like :.2f -> %.2f
                                    char custom_fmt[64];
                                    snprintf(custom_fmt, sizeof(custom_fmt), "%%%s", format_spec); // add custom specifier
                                    
                                    // if it's not a float but they tried to format it, it might still work in C but maybe add float type letter if missing
                                    if (strcmp(vt, "hold.this.float") == 0 && strchr(format_spec, 'f') == NULL) {
                                        strcat(custom_fmt, "f");
                                    }
                                    strcat(translated_str, custom_fmt);
                                } else {
                                    // Fallback to default format for type
                                    strcat(translated_str, get_c_format(vt));
                                }
                                
                                if (!is_first_arg) {
                                    strcat(args_str, ", ");
                                }
                                
                                // Special case bools printing
                                if (strcmp(vt, "hold.this.boolean") == 0) {
                                    char tmp[256];
                                    snprintf(tmp, sizeof(tmp), "%s ? \"true\" : \"false\"", varname);
                                    strcat(args_str, tmp);
                                } else {
                                    strcat(args_str, varname);
                                }
                                
                                is_first_arg = 0;
                            } else {
                                printf("Syntax Error on line %d: Missing closing '}' for variable.\n", line_num);
                                has_error = 1;
                                break;
                            }
                        } else {
                            int len = strlen(translated_str);
                            translated_str[len] = *c;
                            translated_str[len+1] = '\0';
                            c++;
                        }
                    }

                    if (!has_error) {
                        if (strlen(args_str) > 0) {
                           fprintf(out, "    printf(\"%s\\n\", %s);\n", translated_str, args_str);
                        } else {
                           fprintf(out, "    printf(\"%s\\n\");\n", translated_str);
                        }
                    }
                } else {
                     printf("Syntax Error on line %d: Missing closing `\");`\n", line_num);
                     has_error = 1;
                }
            } 
            // Handle regular '...' statements without f-string interpretation
            else if (strncmp(ptr, "hear_my_mans('", 14) == 0) {
                char *str_start = ptr + 14;
                char *end = strstr(str_start, "');");
                if (end) {
                    *end = '\0';
                    fprintf(out, "    printf(\"%%s\\n\", \"%s\");\n", str_start);
                } else {
                    printf("Syntax Error on line %d: Missing closing `');`\n", line_num);
                    has_error = 1;
                }
            } 
            else if (strncmp(ptr, "hear_my_mans(\"", 14) == 0) {
                char *str_start = ptr + 14;
                char *end = strstr(str_start, "\");");
                if (end) {
                    *end = '\0';
                    fprintf(out, "    printf(\"%%s\\n\", \"%s\");\n", str_start);
                } else {
                    printf("Syntax Error on line %d: Missing closing `\");`\n", line_num);
                    has_error = 1;
                }
             } else {
                printf("Syntax Error on line %d: `hear_my_mans` cannot be parsed. Did you forget quotes or f-string prefix?\n", line_num);
                has_error = 1;
             }
        }
        // ==========================
        // 3. Loops: laps(i in range(x, y)){
        // ==========================
        else if (strncmp(ptr, "laps(", 5) == 0) {
            char loop_var[50];
            char start_val[50];
            char end_val[50];
            
            // Expected format: laps(i in range(0,10)){  or similarly spaced
            // Try extracting using sscanf matching the pattern exactly: " %[^ ] in range( %[^,] , %[^)] ) {"
            int matched = sscanf(ptr, "laps ( %49s in range ( %49[^,] , %49[^)] ) ) {", loop_var, start_val, end_val);
            
            // Sometimes they squish it like laps(i in range(0,10)){ so we need a cleaner parser
            if (matched != 3) {
                 matched = sscanf(ptr, "laps(%49s in range(%49[^,],%49[^)])){", loop_var, start_val, end_val);
            }

            if (matched == 3) {
                // Determine loop iter variable automatically as integer
                fprintf(out, "    for(long long %s = %s; %s < %s; %s++) {\n", loop_var, start_val, loop_var, end_val, loop_var);
                
                // Add the new loop iterator explicitly to the compiler's symbol table as an int so hear_my_mans f"{i}" works!
                strcpy(sym_table[var_count].name, loop_var);
                strcpy(sym_table[var_count].type, "hold.this.int"); // Native int type
                var_count++;
            } else {
                 printf("Syntax Error on line %d: Invalid `laps()` loop formatting. Expected `laps(i in range(start, end)){`\n", line_num);
                 has_error = 1;
            }
        // ==========================
        // 4. Closing Braces and miscellaneous
        // ==========================
        } else if (strncmp(ptr, "}", 1) == 0) {
             fprintf(out, "    }\n");
        } else {
            // Unrecognized line
            trim_trailing_spaces(ptr);
             if (strlen(ptr) > 0) {
                 printf("Syntax Error on line %d: Unrecognized statement '%s'\n", line_num, ptr);
                 has_error = 1;
             }
        }
    } // <-- Missing brace to close out the `while (fgets(...))` loop

    if (has_error) {
        fclose(in);
        fclose(out);
        remove(c_file);
        printf("\nCompilation aborted due to syntax errors.\n");
        exit(1);
    }

    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");

    fclose(in);
    fclose(out);

    // Compile & Run
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "gcc %s -o %s", c_file, exe_file);
    
    int ret = system(cmd);
    
    if (ret == 0) {
        char run_cmd[512];
#ifdef _WIN32
        snprintf(run_cmd, sizeof(run_cmd), "%s", exe_file);
#else
        snprintf(run_cmd, sizeof(run_cmd), "./%s", exe_file);
#endif
        system(run_cmd);

        remove(c_file);
        remove(exe_file);
    } else {
        printf("Compilation failed during gcc stage.\n");
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("YN Language Compiler (ync)\n");
        printf("Usage: ync <script.yn>\n");
        return 1;
    }
    compile_file(argv[1]);
    return 0;
}
