#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_executable> <output_header>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    if (!in) {
        printf("Error: Could not open %s\n", argv[1]);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        printf("Error: Could not create %s\n", argv[2]);
        fclose(in);
        return 1;
    }

    fprintf(out, "#ifndef YNC_PAYLOAD_H\n");
    fprintf(out, "#define YNC_PAYLOAD_H\n\n");
    fprintf(out, "const unsigned char ync_payload[] = {\n");

    unsigned char byte;
    int count = 0;
    while (fread(&byte, 1, 1, in) == 1) {
        fprintf(out, "0x%02X, ", byte);
        count++;
        if (count % 16 == 0) {
            fprintf(out, "\n");
        }
    }

    fprintf(out, "\n};\n");
    fprintf(out, "const unsigned int ync_payload_size = %d;\n\n", count);
    fprintf(out, "#endif // YNC_PAYLOAD_H\n");

    fclose(in);
    fclose(out);
    
    printf("Successfully packed %s into %s (%d bytes).\n", argv[1], argv[2], count);
    return 0;
}
