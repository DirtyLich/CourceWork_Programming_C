#include <stdio.h>         // Подключаем заголовочный файл для стандартного ввода-вывода
#include <stdlib.h>        // Подключаем заголовочный файл для работы с памятью
#include <string.h>        // Подключаем заголовочный файл для работы со строками
#include <ctype.h>         // Подключаем заголовочный файл для работы с символами
#include <locale.h>

int isPalindrome(const char *str);                                            
                                                                
void findPalindromes(const char *buffer, int *maxLen, int *start, int *len);    
                                                                
void removePunctuationAndSpaces(char *str);                                     

int main(int argc, char *argv[]) {
        setlocale(LC_ALL, "ru_RU.utf8");

    if (argc != 2) {
        printf("\n|---------------------------------------------------------------------|\n");
                printf("\033[32m");
        printf("Использование программы: ");
                printf("\033[0m");
        printf("%s <имя файла>\n", argv[0]);
        printf("|---------------------------------------------------------------------|\n\n");
        return 1;
    }
        // Открытие файла, имя которого передается в качестве аргумента командной строки
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("\033[1;31m");
        printf("Не удалось открыть файл %s\n", argv[1]);
        printf("\033[0m");
        return 1;
    }

        char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file)) {
        int maxLen = 1; // Длина найденного палиндрома
        int start = 0;  // Начальная позиция найденного палиндрома
        int len = strlen(buffer); // Определяем длину строки

        removePunctuationAndSpaces(buffer);
        findPalindromes(buffer, &maxLen, &start, &len);

        if (maxLen > 1) {
        printf("\n|-------------------------------------|\n");
                printf("\033[32m");
            printf("Найден палиндром: ");
            printf("\033[0m"); // сбрасываем цвет текста в стандартный
            for (int i = start; i < start + maxLen; i++) {
                printf("%c", buffer[i]);
            }
            printf("\n|-------------------------------------|\n");
            printf("\n");
        }
    }

    fclose(file);
    return 0;
}


// Функция, которая проверяет, является ли строка палиндромом
int isPalindrome(const char *str) {
    int len = strlen(str);   // Определяем длину строки
    int i = 0, j = len - 1;  // Инициализируем указатели на начало и конец строки
    while (i < j) {          // Запускаем цикл, пока i меньше j
        while (!isalnum(str[i])) {  // Находим первый алфавитно-цифровой символ слева
            i++;
        }
        while (!isalnum(str[j])) {  // Находим первый алфавитно-цифровой символ справа
            j--;
        }
        if (tolower(str[i]) != tolower(str[j])) {  // Сравниваем символы, игнорируя регистр
            return 0; // Не палиндром
        }
        i++;
        j--;
    }
    return 1; // Палиндром
}

// Функция, которая удаляет пробелы и знаки препинания из строки
void removePunctuationAndSpaces(char *str) {
    int len = strlen(str);   // Определяем длину строки
    char *newStr = (char *)malloc(len * sizeof(char));  // Выделяем память для новой строки
    if (newStr == NULL) {    // Проверяем успешность выделения памяти
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    
    int j = 0;               // Инициализируем индекс для новой строки
    for (int i = 0; i < len; i++) {  // Проходим по символам исходной строки
        if (isalnum(str[i])) {  // Если символ алфавитно-цифровой, копируем его
            newStr[j++] = tolower(str[i]); // Преобразуем символ в нижний регистр
        }
    }
    
    newStr[j] = '\0';         // Завершаем новую строку нулевым символом
    strcpy(str, newStr);      // Заменяем исходную строку на новую
    free(newStr);             // Освобождаем память, выделенную для новой строки
}

void findPalindromes(const char *buffer, int *maxLen, int *start, int *len) {
    // Выделяем память для двумерного массива dp, используемого для хранения информации о палиндромах
    int **dp = (int **)malloc(*len * sizeof(int *));
    for (int i = 0; i < *len; i++) {
        dp[i] = (int *)malloc(*len * sizeof(int));
        memset(dp[i], 0, *len * sizeof(int)); // Инициализируем массив нулями
    }

    // Инициализируем диагональ элементов dp, где строка и столбец имеют одинаковые индексы
    for (int i = 0; i < *len; i++) {
        dp[i][i] = 1; // Один символ всегда считается палиндромом
    }

    // Проверяем пары соседних символов в строке и отмечаем палиндромы длиной 2 символа
    for (int i = 0; i < *len - 1; i++) {
        if (buffer[i] == buffer[i + 1]) {
            dp[i][i + 1] = 1; // Отмечаем пару символов как палиндром
            *maxLen = 2; // Запоминаем длину палиндрома
            *start = i; // Запоминаем начальную позицию палиндрома
        }
    }

    // Поиск палиндромов большей длины с помощью динамического программирования
    for (int k = 3; k <= *len; k++) {
        for (int i = 0; i < *len - k + 1; i++) {
            int j = i + k - 1;
            // Проверяем, является ли подстрока между i и j палиндромом и символы на концах равны
            if (dp[i + 1][j - 1] && buffer[i] == buffer[j]) {
                dp[i][j] = 1; // Отмечаем подстроку как палиндром
                if (k > *maxLen) {
                    *maxLen = k; // Обновляем максимальную длину палиндрома
                    *start = i; // Обновляем начальную позицию палиндрома
                }
            }
        }
    }

    // Освобождаем выделенную память для массива dp
    for (int i = 0; i < *len; i++) {
        free(dp[i]);
    }
    free(dp);
}

