#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char* get_c_format(const char* var_type) {
    if (!var_type) return "%s"; 
    if (strcmp(var_type, "hold.this.character") == 0) return "%c";
    if (strcmp(var_type, "hold.this.sting") == 0 || strcmp(var_type, "hold.this.string") == 0) return "%s";
    if (strcmp(var_type, "hold.this.int") == 0) return "%lld";
    if (strcmp(var_type, "hold.this.boolean") == 0) return "%s"; 
    if (strcmp(var_type, "hold.this.float") == 0) return "%f";
    return "%s";
}

void trim_trailing(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void Parser_parse_line(Parser* self, char* line) {
    char *ptr = line;
    while(isspace((unsigned char)*ptr)) ptr++;
    if (*ptr == '\0' || strncmp(ptr, "//", 2) == 0) return;

    // 1. Variable Declarations
    if (strncmp(ptr, "hold.this.", 10) == 0) {
        char var_type[50];
        char var_name[100];
        char var_val[512];
        
        // Try parsing with assignment first: hold.this.type name = value;
        int matched = sscanf(ptr, "%49s %99s = %511[^;];", var_type, var_name, var_val);
        
        if (matched == 3) {
            // It has an assignment
            if (strcmp(var_type, "hold.this.character") == 0) {
                fprintf(self->out, "    char %s = %s;\n", var_name, var_val);
            } else if (strcmp(var_type, "hold.this.sting") == 0 || strcmp(var_type, "hold.this.string") == 0) {
                fprintf(self->out, "    const char* %s = %s;\n", var_name, var_val);
            } else if (strcmp(var_type, "hold.this.int") == 0) {
                fprintf(self->out, "    long long %s = %s;\n", var_name, var_val);
            } else if (strcmp(var_type, "hold.this.boolean") == 0) {
                fprintf(self->out, "    bool %s = %s;\n", var_name, var_val);
            } else if (strcmp(var_type, "hold.this.float") == 0) {
                fprintf(self->out, "    float %s = %s;\n", var_name, var_val);
            } else {
                printf("Syntax Error on line %d: Unknown type '%s'\n", self->line_num, var_type);
                self->has_error = 1;
            }

            if (!self->has_error) {
                self->sym_table->add(self->sym_table, var_name, var_type);
            }
        } 
        else {
            // Try parsing without assignment: hold.this.type name;
            matched = sscanf(ptr, "%49s %99[^;];", var_type, var_name);
            if (matched == 2) {
                if (strcmp(var_type, "hold.this.character") == 0) {
                    fprintf(self->out, "    char %s = '\\0';\n", var_name);
                } else if (strcmp(var_type, "hold.this.sting") == 0 || strcmp(var_type, "hold.this.string") == 0) {
                    fprintf(self->out, "    const char* %s = \"\";\n", var_name);
                } else if (strcmp(var_type, "hold.this.int") == 0) {
                    fprintf(self->out, "    long long %s = 0;\n", var_name);
                } else if (strcmp(var_type, "hold.this.boolean") == 0) {
                    fprintf(self->out, "    bool %s = false;\n", var_name);
                } else if (strcmp(var_type, "hold.this.float") == 0) {
                    fprintf(self->out, "    float %s = 0.0f;\n", var_name);
                } else {
                    printf("Syntax Error on line %d: Unknown type '%s'\n", self->line_num, var_type);
                    self->has_error = 1;
                }

                if (!self->has_error) {
                    self->sym_table->add(self->sym_table, var_name, var_type);
                }
            } else {
                 printf("Syntax Error on line %d: Variable declaration is malformed.\n", self->line_num);
                 self->has_error = 1;
            }
        }
        return;
    }

    // 1.5 Variable Reassignment
    char check_name[100];
    char reassign_val[512];
    if (sscanf(ptr, "%99s = %511[^;];", check_name, reassign_val) == 2) {
        // Did we find an equals sign? And does the variable exist?
        const char *vt = self->sym_table->get_type(self->sym_table, check_name);
        if (vt != NULL) {
            // Exclude loop variable checks that might accidentally match
            if (strcmp(check_name, "laps") != 0 && strcmp(check_name, "hear_my_mans") != 0) {
                fprintf(self->out, "    %s = %s;\n", check_name, reassign_val);
                return;
            }
        }
    }

    // 2. hear_my_mans Print Statement
    if (strncmp(ptr, "hear_my_mans(", 13) == 0) {
        // Handle f"..."
        if (strncmp(ptr, "hear_my_mans(f\"", 15) == 0) {
            char *str_start = ptr + 15;
            char *end = strstr(str_start, "\");");
            
            if (end) {
                *end = '\0';
                trim_trailing(str_start);
                
                char translated_str[1024] = "";
                char args_str[1024] = "";
                int is_first_arg = 1;
                
                char *c = str_start;
                while (*c != '\0') {
                    if (*c == '{') {
                        c++;
                        char varname[100];
                        char format_spec[50] = "";
                        int i = 0, has_format = 0;
                        
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
                            c++;
                            const char *vt = self->sym_table->get_type(self->sym_table, varname);
                            if (!vt) {
                                printf("Syntax Error on line %d: Variable '%s' is not defined!\n", self->line_num, varname);
                                self->has_error = 1;
                                break;
                            }

                            if (has_format) {
                                char custom_fmt[64];
                                snprintf(custom_fmt, sizeof(custom_fmt), "%%%s", format_spec);
                                if (strcmp(vt, "hold.this.float") == 0 && strchr(format_spec, 'f') == NULL) {
                                    strcat(custom_fmt, "f");
                                }
                                strcat(translated_str, custom_fmt);
                            } else {
                                strcat(translated_str, get_c_format(vt));
                            }
                            
                            if (!is_first_arg) strcat(args_str, ", ");
                            
                            if (strcmp(vt, "hold.this.boolean") == 0) {
                                char tmp[256];
                                snprintf(tmp, sizeof(tmp), "%s ? \"true\" : \"false\"", varname);
                                strcat(args_str, tmp);
                            } else {
                                strcat(args_str, varname);
                            }
                            is_first_arg = 0;
                        } else {
                            printf("Syntax Error on line %d: Missing closing '}' for variable.\n", self->line_num);
                            self->has_error = 1;
                            break;
                        }
                    } else {
                        int len = strlen(translated_str);
                        translated_str[len] = *c;
                        translated_str[len+1] = '\0';
                        c++;
                    }
                }

                if (!self->has_error) {
                    if (strlen(args_str) > 0) {
                       fprintf(self->out, "    printf(\"%s\\n\", %s);\n", translated_str, args_str);
                    } else {
                       fprintf(self->out, "    printf(\"%s\\n\");\n", translated_str);
                    }
                }
            } else {
                 printf("Syntax Error on line %d: Missing closing `\");`\n", self->line_num);
                 self->has_error = 1;
            }
        } 
        else if (strncmp(ptr, "hear_my_mans('", 14) == 0) {
            char *str_start = ptr + 14;
            char *end = strstr(str_start, "');");
            if (end) {
                *end = '\0';
                fprintf(self->out, "    printf(\"%%s\\n\", \"%s\");\n", str_start);
            } else {
                printf("Syntax Error on line %d: Missing closing `');`\n", self->line_num);
                self->has_error = 1;
            }
        } 
        else if (strncmp(ptr, "hear_my_mans(\"", 14) == 0) {
            char *str_start = ptr + 14;
            char *end = strstr(str_start, "\");");
            if (end) {
                *end = '\0';
                fprintf(self->out, "    printf(\"%%s\\n\", \"%s\");\n", str_start);
            } else {
                printf("Syntax Error on line %d: Missing closing `\");`\n", self->line_num);
                self->has_error = 1;
            }
         } else {
            printf("Syntax Error on line %d: `hear_my_mans` cannot be parsed. Did you forget quotes or f-string prefix?\n", self->line_num);
            self->has_error = 1;
         }
         return;
    }
    
    // 3. Loops: laps(i in range(x, y)){
    if (strncmp(ptr, "laps(", 5) == 0) {
        char loop_var[50];
        char start_val[50];
        char end_val[50];
        
        int matched = sscanf(ptr, "laps ( %49s in range ( %49[^,] , %49[^)] ) ) {", loop_var, start_val, end_val);
        if (matched != 3) {
             matched = sscanf(ptr, "laps(%49s in range(%49[^,],%49[^)])){", loop_var, start_val, end_val);
        }

        if (matched == 3) {
            fprintf(self->out, "    for(long long %s = %s; %s < %s; %s++) {\n", loop_var, start_val, loop_var, end_val, loop_var);
            self->sym_table->add(self->sym_table, loop_var, "hold.this.int");
        } else {
             printf("Syntax Error on line %d: Invalid `laps()` loop formatting. Expected `laps(i in range(start, end)){`\n", self->line_num);
             self->has_error = 1;
        }
        return;
    }
    
    // 4. Closing Braces
    if (strncmp(ptr, "}", 1) == 0) {
         fprintf(self->out, "    }\n");
         return;
    }
    
    // Unrecognized
    trim_trailing(ptr);
    if (strlen(ptr) > 0) {
         printf("Syntax Error on line %d: Unrecognized statement '%s'\n", self->line_num, ptr);
         self->has_error = 1;
    }
}

void Parser_parse_file(Parser* self) {
    char line[1024];
    while (fgets(line, sizeof(line), self->in)) {
        self->line_num++;
        self->parse_line(self, line);
        if (self->has_error) break;
    }
}

Parser* create_parser(FILE* in, FILE* out, SymbolTable* st) {
    Parser* p = (Parser*)malloc(sizeof(Parser));
    p->in = in;
    p->out = out;
    p->sym_table = st;
    p->line_num = 0;
    p->has_error = 0;
    p->parse_line = Parser_parse_line;
    p->parse_file = Parser_parse_file;
    return p;
}

void destroy_parser(Parser* p) {
    if (p) free(p);
}
