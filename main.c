#include <stdio.h>  // Подключение библиотеки для работы с вводом/выводом
#include <stdlib.h> // Подключение библиотеки для работы с функциями общего назначения
#include <string.h> // Подключение библиотеки для работы со строками

#define WINDOW_SIZE 32 // Определение размера скользящего окна
#define BUFFER_SIZE 16  // Определение размера буфера

typedef struct
{ // Определение структуры Token
    int offset;
    int length;
    char next;
} Token;
Token token = {0}; // Инициализация структуры Token нулями

void compress(FILE *input, FILE *output)
{                             // Функция для сжатия файла
    char window[WINDOW_SIZE]; // Объявление массива для скользящего окна
    char buffer[BUFFER_SIZE]; // Объявление массива для буфера
    Token token;              // Объявление структуры Token

    memset(window, 0, sizeof(window)); // Заполнение массива window нулями
    memset(buffer, 0, sizeof(buffer)); // Заполнение массива buffer нулями

    while (fgets(buffer, BUFFER_SIZE, input) != NULL)
    {                                      // Чтение файла построчно в буфер
        int bufferLength = strlen(buffer); // Определение длины буфера
        int i = 0;                         // Инициализация переменной i

        while (i < bufferLength)
        {                            // Цикл по буферу
            int j = i - WINDOW_SIZE; // Определение начала скользящего окна
            if (j < 0)
            {          // Если начало окна меньше нуля
                j = 0; // То начало окна равно нулю
            }

            int maxLength = 0; // Инициализация переменной maxLength
            int maxOffset = 0; // Инициализация переменной maxOffset

            while (j < i)
            {                   // Цикл по скользящему окну
                int length = 0; // Инициализация переменной length

                char *match = memchr(buffer + i, buffer[j], BUFFER_SIZE - i);
                    if (match != NULL)
                        {
                            length = match - (buffer + i);
                        }
                    else
                        {
                            length = BUFFER_SIZE - i;
                        }

                // while (buffer[i + length] == buffer[j + length] && length < BUFFER_SIZE)
                // {             // Поиск повторяющихся фрагментов
                //     length++; // Увеличение длины повторяющегося фрагмента
                // }

                if (length > maxLength)
                {                       // Если длина повторяющегося фрагмента больше максимальной длины
                    maxLength = length; // То максимальная длина равна длине повторяющегося фрагмента
                    maxOffset = i - j;  // А смещение равно разности позиций текущего и предыдущего вхождения фрагмента
                }

                j++; // Увеличение счетчика j
            }

            token.offset = maxOffset;           // Запись смещения в структуру Token
            token.length = maxLength;           // Запись длины в структуру Token
            token.next = buffer[i + maxLength]; // Запись следующего символа в структуру Token

            fwrite(&token, sizeof(Token), 1, output); // Запись структуры Token в выходной файл

            i += maxLength + 1; // Увеличение счетчика i
        }
    }
}

void decompress(FILE *input, FILE *output)
{                             // Функция для распаковки файла
    char window[WINDOW_SIZE]; // Объявление массива для скользящего окна
    Token token;              // Объявление структуры Token

    memset(window, 0, sizeof(window)); // Заполнение массива window нулями

    while (fread(&token, sizeof(Token), 1, input) == 1)
    { // Чтение структуры Token из входного файла
        int i;
        for (i = 0; i < token.length; i++)
        {                                                 // Цикл по длине повторяющегося фрагмента
            char c = window[token.offset + i];            // Получение символа из скользящего окна
            fwrite(&c, sizeof(char), 1, output);          // Запись символа в выходной файл
            window[WINDOW_SIZE - 1] = c;                  // Добавление символа в конец скользящего окна
            memmove(window, window + 1, WINDOW_SIZE - 1); // Сдвиг скользящего окна на один символ влево
        }

        fwrite(&token.next, sizeof(char), 1, output); // Запись следующего символа в выходной файл
        window[WINDOW_SIZE - 1] = token.next;         // Добавление следующего символа в конец скользящего окна
        memmove(window, window + 1, WINDOW_SIZE - 1); // Сдвиг скользящего окна на один символ влево
    }
}

int main(int argc, char *argv[])
{ // Главная функция программы
    if (argc < 5)
    {                                                                    // Если количество аргументов меньше 5
        printf("Usage: lz77compress -c/-d -o output_file input_file\n"); // Вывод сообщения об использовании программы
        return 1;                                                        // Возврат кода ошибки
    }

    char *option = argv[1];         // Получение опции из аргументов командной строки
    char *outputFileName = argv[3]; // Получение имени выходного файла из аргументов командной строки
    char *inputFileName = argv[4];  // Получение имени входного файла из аргументов командной строки

    FILE *inputFile = fopen(inputFileName, "r");   // Открытие входного файла для чтения
    FILE *outputFile = fopen(outputFileName, "w"); // Открытие выходного файла для записи

    if (inputFile == NULL || outputFile == NULL)
    {                                    // Если не удалось открыть входной или выходной файл
        printf("Error opening files\n"); // Вывод сообщения об ошибке
        return 1;                        // Возврат кода ошибки
    }

    if (strcmp(option, "-c") == 0)
    {                                    // Если опция -c
        compress(inputFile, outputFile); // Вызов функции сжатия
    }
    else if (strcmp(option, "-d") == 0)
    {                                      // Если опция -d
        decompress(inputFile, outputFile); // Вызов функции распаковки
    }
    else
    {                               // Если опция некорректна
        printf("Invalid option\n"); // Вывод сообщения об ошибке
        return 1;                   // Возврат кода ошибки
    }

    fclose(inputFile);  // Закрытие входного файла
    fclose(outputFile); // Закрытие выходного файла

    return 0; // Возврат нулевого кода, если программа завершилась без ошибок
}