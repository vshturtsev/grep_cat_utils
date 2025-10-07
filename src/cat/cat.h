#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/common.h"

#define STR_BUFFER 256   // size for malloc and additional for realloc
#define STR_THRESHOLD 16 // bound size for realloc

typedef struct Flags {
  char b; // нумерует только непустые строки
  char e; // также отображает символы конца строки как $
  char t; // также отображает табы как ^I
  char s; // сжимает несколько смежных пустых строк
  char n; // нумерует все выходные строки
  char v; // меняет непечатные символы
} Flags;

int read_long_flag(Flags *fl, char *arg);
int read_flags(Flags *fl, char *arg);
void add_symbol(Flags fl, int ch, char **str, int *p_i);
int read_str(Flags fl, FILE *file, char **str, int *str_len);
int cat_file(Flags fl, char *filename);
int is_noprint(int ch);
void print_str(Flags fl, char *strbuffer, int str_len, int file_state);
int check_file(const char *filename);

// testinf functions
void print_flags(Flags fl);

void print_flags(Flags fl) {
  printf("b = %d\ne = %d\nt = %d\ns = %d\nn = %d\nv = %d\n", fl.b, fl.e, fl.t,
         fl.s, fl.n, fl.v);
}
