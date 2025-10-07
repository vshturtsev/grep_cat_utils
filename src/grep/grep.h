#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/common.h"

#define STR_BUFFER 256   // size for malloc and additional for realloc
#define STR_THRESHOLD 16 // bound size for realloc

typedef struct Flags {
  // char e;  //	Шаблон.
  char i; //	Игнорирует различия регистра.
  char v; //	Инвертирует смысл поиска соответствий.
  char c; //	Выводит только количество совпадающих строк.
  char l; //	Выводит только совпадающие файлы.
  char n; //	Предваряет каждую строку вывода номером строки из файла ввода.
  char h; //	Выводит совпадающие строки, не предваряя их именами файлов.
  char s; //	Подавляет сообщения об ошибках о несуществующих или нечитаемых
          //файлах.
  // char f;  // file	Получает регулярные выражения из файла.
  char o; //	Печатает только совпадающие (непустые) части совпавшей строки.
} Flags;

// 1 if not exist, -1 if empty, 0 is ok
int check_file(const char *filename);

// return: 1 if not exist, -1 if empty, -2 if binary, 0 is ok
int check_file_for_grep(const char *filename, Flags fl);

int resize_str(char **str, int s);
int get_str(FILE *file, char **str, size_t *size);
int read_patterns_file(char ***patterns, int *patterns_count,
                       const char *filename);
char *strarray_to_pattern(char **array, int arr_size);
int get_flag(Flags *fl, char ch);
int read_args(int argc, char *argv[], Flags *fl, char ***files,
              int *files_count, char ***patterns, int *patterns_count);
void print_str(char *str, char *filename, int files_count, size_t str_num,
               Flags fl);
int compare_str(Flags fl, char *filename, int files_count, char *str,
                int str_num, char **patterns, int pat_count,
                size_t *grep_count);
int check_void_pattern(char **patterns, int pat_count);
int count_void(FILE *file);
int grep_file(Flags fl, char *filename, int files_count, char **patterns,
              int pat_count);
int check_ef_flag(int argc, char *argv[]);
int read_ef_flag(int argc, char *argv[], char ***patterns, int *patterns_count,
                 int *i, int *j);