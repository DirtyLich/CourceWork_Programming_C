#include <stdio.h>
#include <string.h>

#define WINDOW_SIZE 4096
#define LOOKAHEAD_SIZE 16

typedef struct {
    int offset;
    int length;
} pair;

void encode(const char* input, int input_size, pair* output, int* output_size) {
    int next_offset = 0;
    *output_size = 0;
    while (next_offset < input_size) {
        int max_length = 0;
        int max_offset = 0;
        for (int offset = 1; offset <= WINDOW_SIZE && next_offset - offset >= 0; offset++) {
            int length = 0;
            while (length < LOOKAHEAD_SIZE && next_offset + length < input_size && input[next_offset - offset + length] == input[next_offset + length]) {
                length++;
            }
            if (length > max_length) {
                max_length = length;
                max_offset = offset;
            }
        }
        output[*output_size].offset = max_offset;
        output[*output_size].length = max_length;
        (*output_size)++;
        next_offset += max_length;
    }
}

void decode(const pair* pairs, int pair_count, const char* input, char* output) {
    int next_offset = 0;
    for (int i = 0; i < pair_count; i++) {
        int offset = pairs[i].offset;
        int length = pairs[i].length;
        for (int j = 0; j < length; j++) {
            output[next_offset + j] = input[next_offset - offset + j];
        }
        next_offset += length;
    }
    output[next_offset] = '\0';
}

int main() {
    const char* input = "abracadabrabracadabrabracadabrabracadabra";
    int input_size = strlen(input);
    pair output[100];
    int output_size;
    encode(input, input_size, output, &output_size);
    printf("Encoded %d pairs:\n", output_size);
    for (int i = 0; i < output_size; i++) {
        printf("(%d, %d) ", output[i].offset, output[i].length);
    }
    printf("\n");
    char decoded[100];
    decode(output, output_size, input, decoded);
    printf("Decoded: %s\n", decoded);
    return 0;
}