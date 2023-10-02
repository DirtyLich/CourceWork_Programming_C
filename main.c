#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_SIZE 32
#define BUFFER_SIZE 16

typedef struct
{
    int offset;
    int length;
    char next;
} Token;

void compress(FILE *input, FILE *output)
{
    char window[WINDOW_SIZE];
    char buffer[BUFFER_SIZE];
    Token token;

    memset(window, 0, sizeof(window));
    memset(buffer, 0, sizeof(buffer));

    int bufferLength = 0;

    while ((bufferLength = fread(buffer, 1, BUFFER_SIZE, input)) > 0)
    {
        int i = 0;

        while (i < bufferLength)
        {
            int j = i - WINDOW_SIZE;
            if (j < 0)
            {
                j = 0;
            }

            int maxLength = 0;
            int maxOffset = 0;

            while (j < i)
            {
                int length = 0;
                int k = 0;
                while (i + k < bufferLength && buffer[j + k] == buffer[i + k])
                {
                    length++;
                    k++;
                }

                if (length > maxLength)
                {
                    maxLength = length;
                    maxOffset = i - j;
                }

                j++;
            }

            token.offset = maxOffset;
            token.length = maxLength;
            token.next = buffer[i + maxLength];

            fwrite(&token, sizeof(Token), 1, output);

            i += maxLength + 1;
        }
        char literal = buffer[bufferLength - 1];
        if (literal == 0)
        {
            fputc(0, output); // Используем 0 для обозначения литерала
        }
        else
        {
            fputc(literal, output); // Записываем литерал как есть
        }
    }
}

void decompress(FILE *input, FILE *output)
{
    char window[WINDOW_SIZE];
    Token token;

    memset(window, 0, sizeof(window));

    int isLiteral = 0; // Флаг для определения литерала

    while (fread(&token, sizeof(Token), 1, input) == 1)
    {
        if (isLiteral)
        {
            // Если предыдущий символ был литералом, восстанавливаем его
            char c = token.next;
            fwrite(&c, sizeof(char), 1, output);
            window[WINDOW_SIZE - 1] = c;
            memmove(window, window + 1, WINDOW_SIZE - 1);
            isLiteral = 0;
        }
        else if (token.length == 0)
        {
            char c = token.next;
            if (c == 0)
            {
                // Если текущий символ - литерал, устанавливаем флаг
                isLiteral = 1;
            }
            else
            {
                fwrite(&c, sizeof(char), 1, output);
                window[WINDOW_SIZE - 1] = c;
                memmove(window, window + 1, WINDOW_SIZE - 1);
            }
        }
        else
        {
            int i;
            for (i = 0; i < token.length; i++)
            {
                char c = window[token.offset + i];
                fwrite(&c, sizeof(char), 1, output);
                window[WINDOW_SIZE - 1] = c;
                memmove(window, window + 1, WINDOW_SIZE - 1);
            }
            if (token.next != 0)
            {
                fwrite(&token.next, sizeof(char), 1, output);
            }
            window[WINDOW_SIZE - 1] = token.next;
            memmove(window, window + 1, WINDOW_SIZE - 1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Usage: lz77 -c/-d -o output_file input_file\n");
        return 1;
    }

    char *option = argv[1];
    char *outputFileName = argv[3];
    char *inputFileName = argv[4];

    FILE *inputFile = fopen(inputFileName, "rb");
    FILE *outputFile = fopen(outputFileName, "wb");

    if (inputFile == NULL || outputFile == NULL)
    {
        printf("Error opening files\n");
        return 1;
    }

    if (strcmp(option, "-c") == 0)
    {
        compress(inputFile, outputFile);
    }
    else if (strcmp(option, "-d") == 0)
    {
        decompress(inputFile, outputFile);
    }
    else
    {
        printf("Invalid option\n");
        return 1;
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
