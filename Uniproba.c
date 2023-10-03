#include <stdio.h>         // Подключаем заголовочный файл для стандартного ввода-вывода
#include <stdlib.h>        // Подключаем заголовочный файл для работы с памятью
#include <string.h>        // Подключаем заголовочный файл для работы со строками
#include <ctype.h>         // Подключаем заголовочный файл для работы с символами
#include <wctype.h>     // Для работы с символами Unicode
#include <wchar.h>      // Для работы с широкими символами
#include <locale.h>

int isPalindrome(const wchar_t *str);

void findPalindromes(const wchar_t *buffer, int *maxLen, int *start, int *len);

void removePunctuationAndSpaces(wchar_t *str);

void SayHiForUser();


int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8"); // Устанавливаем локаль для русского языка с кодировкой UTF-8

    if (argc != 2) {
        wprintf(L"Использование: %s <имя файла>\n", argv[0]);
        return 1;
    }

    wchar_t *wideFileName = (wchar_t *)malloc((strlen(argv[1]) + 1) * sizeof(wchar_t));
    mbstowcs(wideFileName, argv[1], strlen(argv[1]) + 1);
    FILE *file = _wfopen(wideFileName, L"r, ccs=UTF-8");
    free(wideFileName);


    wchar_t buffer[100000];

    while (fgetws(buffer, sizeof(buffer), file)) {
        int maxLen = 1; // Длина найденного палиндрома
        int start = 0;  // Начальная позиция найденного палиндрома
        int len = wcslen(buffer); // Определение длины строки

        removePunctuationAndSpaces(buffer);
        findPalindromes(buffer, &maxLen, &start, &len);

        if (maxLen > 1) {
            wprintf(L"Найден палиндром: ");
            for (int i = start; i < start + maxLen; i++) {
                wprintf(L"%lc", buffer[i]);
            }
            wprintf(L"\n");
        }
    }

    fclose(file);
    return 0;
}


// Функция, которая проверяет, является ли строка палиндромом
int isPalindrome(const wchar_t *str) {
    int len = wcslen(str);
    int i = 0, j = len - 1;
    while (i < j) {
        while (!iswalnum(str[i])) {
            i++;
        }
        while (!iswalnum(str[j])) {
            j--;
        }
        if (towlower(str[i]) != towlower(str[j])) {
            return 0;
        }
        i++;
        j--;
    }
    return 1;
}

void removePunctuationAndSpaces(wchar_t *str) {
    int len = wcslen(str);   // Определяем длину строки
    wchar_t *newStr = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));  // Выделяем память для новой строки
    if (newStr == NULL) {    // Проверяем успешность выделения памяти
        wprintf(L"Ошибка выделения памяти\n");
        exit(1);
    }
    
    int j = 0;               // Инициализируем индекс для новой строки
    for (int i = 0; i < len; i++) {  // Проходим по символам исходной строки
        if (iswalnum(str[i]) || iswspace(str[i])) {  // Если символ алфавитно-цифровой или пробел, копируем его
            newStr[j++] = towlower(str[i]); // Преобразуем символ в нижний регистр
        }
    }
    
    newStr[j] = L'\0';         // Завершаем новую строку нулевым символом
    wcscpy(str, newStr);      // Заменяем исходную строку на новую
    free(newStr);             // Освобождаем память, выделенную для новой строки
}

void findPalindromes(const wchar_t *buffer, int *maxLen, int *start, int *len) {
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


// void SayHiForUser()
// {
//      printf   
// }