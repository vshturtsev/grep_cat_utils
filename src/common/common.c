#include "common.h"

int add_str_in_array(char ***array, int *size, const char *str) {
  int res = 0;
  char **tmp = realloc(*array, sizeof(char *) * (1 + *size));
  if (tmp != NULL) {
    *array = tmp;
    int len = strlen(str);
    char *ptr = malloc(sizeof(char) * len + 1);
    if (ptr != NULL) {
      strcpy(ptr, str);
      (*array)[*size] = ptr;
      (*size)++;
    } else {
      fprintf(stderr, "ERROR: Memory allocate error\n");
      res = 1;
    }
  } else {
    fprintf(stderr, "ERROR: Memory allocate error\n");
    res = 1;
  }
  return res;
}

void free_strarray(char **array, int size) {
  for (int i = 0; i < size; i++) {
    free(array[i]);
  }
  free(array);
}

int resize_str(char **str, int s) {
  int error = 0;
  char *tmp = realloc((*str), sizeof(char) * STR_BUFFER * s);
  if (tmp == NULL) {
    fprintf(stderr, "ERROR(read_str): Memory allocation error\n");
    error = 1;
  } else {
    *str = tmp;
  }
  return error;
}