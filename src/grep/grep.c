#include "grep.h"

#define STR_BUFFER 256   // size for malloc and additional for realloc
#define STR_THRESHOLD 16 // bound size for realloc

int main(int argc, char *argv[]) {
  Flags fl = {0};
  int error = 0; // error flag
  int files_count = 0, patterns_count = 0;
  char **files = NULL, **patterns = NULL;
  error = read_args(argc, argv, &fl, &files, &files_count, &patterns,
                    &patterns_count);

  if (files_count == 0 || patterns_count == 0) {
    if (!fl.s)
      fprintf(stderr, "No files or patterns to grep\n");
    error = 1;
  }
  if (error == 0) {
    for (int i = 0; i < files_count && !error; i++) {
      error = grep_file(fl, files[i], files_count, patterns, patterns_count);
    }
  }
  // free memory
  if (files != NULL)
    free_strarray(files, files_count);
  if (patterns != NULL)
    free_strarray(patterns, patterns_count);

  return error;
}

// 1 if not exist, -1 if empty, 0 is ok
int check_file(const char *filename) {
  char res = 0;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    res = 1;
  } else {
    fseek(file, 0L, SEEK_END);
    long int cursor = ftell(file);
    if (cursor < 1)
      res = -1;
    fclose(file);
  }
  return res;
}

// return: 1 if not exist, -1 if empty, -2 if binary, 0 is ok
int check_file_for_grep(const char *filename, Flags fl) {
  char res = 0;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    if (!fl.s)
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    res = 1;
  } else {
    fseek(file, 0L, SEEK_END);
    long int cursor = ftell(file);
    if (cursor < 1)
      res = -1;
    else {
      fseek(file, 0L, SEEK_SET);
      int ch = fgetc(file);
      while (ch != EOF && ch != '\0')
        ch = fgetc(file);
      if (ch == '\0')
        res = -2;
    }
    fclose(file);
  }
  return res;
}

int get_str(FILE *file, char **str, size_t *size) {
  int result = 0;
  int s = 1; // s*STR_BUFFER - size of string
  (*str) = calloc(STR_BUFFER * s, sizeof(char));
  if (*str != NULL) {
    int ch = getc(file);
    for (; ch != '\n' && ch != EOF && result == 0;) {
      (*str)[(*size)++] = ch;
      ch = getc(file);
      if ((STR_BUFFER * s) - *size < STR_THRESHOLD) {
        s++;
        result = resize_str(str, s);
      }
    }
    if (result == 0) {
      (*str)[(*size)] = '\0';
      if (ch == EOF)
        result = -1;
    }
  } else
    result = 1;
  return result;
}

int read_patterns_file(char ***patterns, int *patterns_count,
                       const char *filename) {
  int error = 0;
  int check = check_file(filename);
  if (check == 0) {
    FILE *file = fopen(filename, "r");
    fseek(file, 0L, SEEK_SET);
    int file_state = 0;
    while (!file_state && !error) {
      char *str = NULL;
      size_t size = 0;
      file_state = get_str(file, &str, &size);
      if (file_state == 1)
        error = 1;
      else if (!(file_state == -1 && str[0] == '\0'))
        error = add_str_in_array(patterns, patterns_count, str);
      if (str != NULL)
        free(str);
    }
    fclose(file);
  } else if (check == 1) {
    error = 1;
  }
  return error;
}

char *strarray_to_pattern(char **array, int arr_size) {
  int s = 1;
  char *pattern = calloc(STR_BUFFER, sizeof(char));
  if (pattern != NULL) {
    int error = 0;
    for (int i = 0; i < arr_size && !error; i++) {
      if (STR_BUFFER * s - strlen(pattern) < strlen(array[i]) + STR_THRESHOLD) {
        s++;
        error = resize_str(&pattern, s);
      }
      if (!error) {
        if (array[i][0]) {
          strcat(pattern, array[i]);
          if (i < arr_size - 1)
            strcat(pattern, "|");
        }
      } else {
        free(pattern);
        pattern = NULL;
      }
    }
  }
  return pattern;
}

int get_flag(Flags *fl, char ch) {
  int error = 0;
  switch (ch) {
  case 'i':
    fl->i = 1;
    break;
  case 'v':
    fl->v = 1;
    break;
  case 'c':
    fl->c = 1;
    break;
  case 'l':
    fl->l = 1;
    break;
  case 'n':
    fl->n = 1;
    break;
  case 'h':
    fl->h = 1;
    break;
  case 's':
    fl->s = 1;
    break;
  case 'o':
    fl->o = 1;
    break;
  default:
    fprintf(stderr, "ERROR (read_flags): invalid option '%c'\n", ch);
    error = 1;
    break;
  }
  return error;
}

int check_ef_flag(int argc, char *argv[]) {
  int res = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      int len = strlen(argv[i]);
      for (int j = 1; j < len; j++) {
        if (argv[i][j] == 'e' || argv[i][j] == 'f')
          res = 1;
      }
    }
  }
  return res;
}

int read_ef_flag(int argc, char *argv[], char ***patterns, int *patterns_count,
                 int *i, int *j) {
  int error = 0;
  char flag = (argv[*i][*j] == 'e') ? 'e' : 'f';
  if (argv[*i][*j + 1] == '\0') {
    if (*i + 1 < argc) {
      (*i)++;
      error = (flag == 'e')
                  ? add_str_in_array(patterns, patterns_count, argv[*i])
                  : read_patterns_file(patterns, patterns_count, argv[*i]);
    } else {
      fprintf(stderr, "grep: option requires an argument -- '%c'\n", flag);
      error = 1;
    }
  } else {
    int len = strlen(argv[*i]);
    (*j)++;
    char buffer[len - (*j)];
    *buffer = 0;
    for (int k = 0; (*j) < len; (*j)++, k++) {
      buffer[k] = argv[*i][*j];
    }
    error = (flag == 'e')
                ? add_str_in_array(patterns, patterns_count, buffer)
                : read_patterns_file(patterns, patterns_count, buffer);
  }
  return error;
}

int read_args(int argc, char *argv[], Flags *fl, char ***files,
              int *files_count, char ***patterns, int *patterns_count) {
  int error = 0;
  char flag_pattern = check_ef_flag(argc, argv);
  for (int i = 1; i < argc && error == 0; i++) {
    if (argv[i][0] == '-') {
      int len = strlen(argv[i]);
      for (int j = 1; j < len && error == 0; j++) {
        if (argv[i][j] == 'e' || argv[i][j] == 'f') {
          flag_pattern = 1;
          error = read_ef_flag(argc, argv, patterns, patterns_count, &i, &j);
        } else {
          error = get_flag(fl, argv[i][j]);
        }
      }
    } else if (!flag_pattern) {
      flag_pattern = 1;
      error = add_str_in_array(patterns, patterns_count, argv[i]);
    } else {
      error = add_str_in_array(files, files_count, argv[i]);
    }
  }
  return error;
}

void print_str(char *str, char *filename, int files_count, size_t str_num,
               Flags fl) {
  if (!fl.c && !fl.l) {
    if (files_count > 1 && !fl.h)
      printf("%s:", filename);
    if (fl.n)
      printf("%zu:", str_num);
    printf("%s\n", str);
  }
}

int compare_str(Flags fl, char *filename, int files_count, char *str,
                int str_num, char **patterns, int pat_count,
                size_t *grep_count) {
  int reg_error = 0, str_state = 0;
  int cflags = (fl.i) ? REG_ICASE : 0;
  regex_t re;
  if (!fl.o || (fl.o && (fl.c || fl.l))) {
    for (int i = 0; i < pat_count && !reg_error; i++) {
      if ((reg_error = regcomp(&re, patterns[i], cflags)) == 0) {
        str_state = (regexec(&re, str, 0, NULL, 0) == 0) ? 1 : str_state;
      }
    }
    regfree(&re);
  } else if (!fl.v) {
    char *pattern = strarray_to_pattern(patterns, pat_count);
    if (pattern == NULL) {
      reg_error = 1;
    } else {
      cflags |= REG_EXTENDED;
      if ((reg_error = regcomp(&re, pattern, cflags)) == 0) {
        regmatch_t pmatch;
        int start = 0;
        while (regexec(&re, str + start, 1, &pmatch, 0) == 0) {
          str_state = 1;
          char *buffer = calloc(pmatch.rm_eo - pmatch.rm_so + 2, sizeof(char));
          int j = 0;
          for (int i = pmatch.rm_so; i < pmatch.rm_eo; i++, j++) {
            buffer[j] = str[start + i];
          }
          buffer[j + 1] = '\0';
          print_str(buffer, filename, files_count, str_num, fl);
          free(buffer);
          start += pmatch.rm_eo;
        }
      }
      free(pattern);
      regfree(&re);
    }
  }
  if (!reg_error) {
    if (fl.v)
      str_state = (str_state == 0) ? 1 : 0;
    if (str_state) {
      (*grep_count)++;
      if (!fl.o)
        print_str(str, filename, files_count, str_num, fl);
    }
  }
  return reg_error;
}

int check_void_pattern(char **patterns, int pat_count) {
  int res = 0;
  for (int i = 0; i < pat_count; i++) {
    if (patterns[i][0] == '\0')
      res = 1;
  }
  return res;
}

int count_void(FILE *file) {
  int res = 0;
  char ch = getc(file);
  while (ch != -1) {
    if (ch == '\0' || ch == '\n')
      res++;
    ch = getc(file);
  }
  return res;
}

int grep_file(Flags fl, char *filename, int files_count, char **patterns,
              int pat_count) {
  int error = 0; // error flag
  int file_state = check_file_for_grep(filename, fl);
  // error = (file_state > 0) ? 1 : 0;
  if (file_state <= 0) {
    FILE *file = fopen(filename, "r");
    fseek(file, 0L, SEEK_SET);
    size_t grep_count = 0, str_num = 0;
    if (file_state == -2) {
      if ((fl.c || fl.l) &&
          ((check_void_pattern(patterns, pat_count) && !fl.v) ||
           (!check_void_pattern(patterns, pat_count) && fl.v))) {
        grep_count = count_void(file);
      } else if (!fl.s && !fl.c)
        fprintf(stderr, "grep: %s: binary file matches\n", filename);
    } else {
      while (error == 0 && file_state == 0) {
        size_t size = 0;
        str_num++;
        char *str = NULL;
        file_state = get_str(file, &str, &size);
        if (file_state == 1)
          error = 1;
        else if (!(file_state == -1 && str[0] == '\0'))
          error = compare_str(fl, filename, files_count, str, str_num, patterns,
                              pat_count, &grep_count);
        if (str != NULL)
          free(str);
      }
    }
    if (!error) {
      if (fl.l) {
        if (grep_count > 0)
          printf("%s\n", filename);
      } else if (fl.c) {
        if (files_count > 1 && !fl.h)
          printf("%s:", filename);
        printf("%zu\n", grep_count);
      }
    }
    fclose(file);
  }
  return error;
}
