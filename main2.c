#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_SIZE 4096
#define LOOKAHEAD_SIZE 18
#define EOF_SYMBOL 256

typedef struct {
    int offset;
    int length;
    char next;
} lz77_token;

void compress(FILE *input_file, FILE *output_file) {
    char window[WINDOW_SIZE];
    char lookahead[LOOKAHEAD_SIZE];
    int window_pos = 0;
    int lookahead_pos = 0;
    int i, j, k;
    lz77_token token;

    memset(window, 0, WINDOW_SIZE);
memset(lookahead, 0, LOOKAHEAD_SIZE);

    while (1) {
        token.offset = 0;
        token.length = 0;

        for (i = 0; i < LOOKAHEAD_SIZE; i++) {
            lookahead[i] = fgetc(input_file);
            if (lookahead[i] == EOF) {
                lookahead[i] = EOF_SYMBOL;
                break;
            }
        }

        if (i == 0) {
            break;
        }

        for (i = LOOKAHEAD_SIZE - 1; i >= 0; i--) {
            for (j = window_pos; j < WINDOW_SIZE; j++) {
                if (window[j] == lookahead[i]) {
                    for (k = 1; k < LOOKAHEAD_SIZE - i; k++) {
                        if (window[j + k] != lookahead[i + k]) {
                            break;
                        }
                    }
                    if (k > token.length) {
                        token.offset = j - window_pos;
                        token.length = k;
                        token.next = lookahead[i + k];
                    }
                }
            }
        }

        fwrite(&token, sizeof(lz77_token), 1, output_file);

        for (i = 0; i < token.length + 1; i++) {
            window[window_pos] = lookahead[i];
            window_pos = (window_pos + 1) % WINDOW_SIZE;
        }
        lookahead_pos = i;
    }
}

void decompress(FILE *input_file, FILE *output_file) {
    char window[WINDOW_SIZE];
    int window_pos = 0;
    lz77_token token;

    memset(window, ' ', WINDOW_SIZE);

    while (fread(&token, sizeof(lz77_token), 1, input_file) == 1) {
        int i;
        for (i = 0; i <= token.length; i++) {
            char c = window[(window_pos - token.offset + i + WINDOW_SIZE) % WINDOW_SIZE];
            fputc(c, output_file);
            window[window_pos] = c;
            window_pos = (window_pos + 1) % WINDOW_SIZE;
        }
        window[window_pos] = token.next;
        window_pos = (window_pos + 1) % WINDOW_SIZE;
    }
}

int main(int argc, char **argv) {
    FILE *input_file, *output_file;
    int compress_flag = 0, decompress_flag = 0;
    char *input_filename, *output_filename;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            compress_flag = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            decompress_flag = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            output_filename = argv[++i];
        } else {
            input_filename = argv[i];
        }
    }

    if (compress_flag) {
        input_file = fopen(input_filename, "rb");
        output_file = fopen(output_filename, "wb");
        compress(input_file, output_file);
        fclose(input_file);
        fclose(output_file);
    } else if (decompress_flag) {
        input_file = fopen(input_filename, "rb");
        output_file = fopen(output_filename, "wb");
        decompress(input_file, output_file);
        fclose(input_file);
        fclose(output_file);
    }

    return 0;
}