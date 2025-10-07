#include "cat.h"

int main(int argc, char *argv[]) {
  Flags fl = {0};
  int error = 0; // error flag
  int files_count = 0;
  char **files = NULL; // pointer to files array

  for (int i = 1; i < argc && error == 0; i++) {
    if (argv[i][0] == '-' && argv[i][1] == '-') // instead strncmp
      error = read_long_flag(&fl, argv[i]);
    else if (argv[i][0] == '-')
      error = read_flags(&fl, argv[i]);
    else
      error = add_str_in_array(&files, &files_count, argv[i]);
  }
  int nonexist_files = 0;
  if (error == 0) {
    for (int i = 0; i < files_count && !error; i++) {
      if (check_file(files[i]) == 1)
        nonexist_files = 1;
      else if (!check_file(files[i]))
        error = cat_file(fl, files[i]);
    }
  }
  if (files != NULL)
    free_strarray(files, files_count); // free filelist memory
  if (nonexist_files > 0)
    error = 1;
  return error;
}

int read_long_flag(Flags *fl, char *arg) {
  int error = 0;
  if (!strcmp(arg, "--number")) {
    fl->n = 1;
  } else if (!strcmp(arg, "--number-nonblank")) {
    fl->b = 1;
  } else if (!strcmp(arg, "--squeeze-blank")) {
    fl->s = 1;
  } else {
    fprintf(stderr, "ERROR (read_long_flags): invalid option '%s'\n", arg);
    error = 1;
  }
  return error;
}

int read_flags(Flags *fl, char *arg) {
  int error = 0;
  for (int i = 1; arg[i] && error == 0; i++) {
    switch (arg[i]) {
    case 'b':
      fl->b = 1;
      break;
    case 'e':
      fl->e = 1;
      fl->v = 1;
      break;
    case 'E':
      fl->e = 1;
      break;
    case 't':
      fl->t = 1;
      fl->v = 1;
      break;
    case 'T':
      fl->t = 1;
      break;
    case 's':
      fl->s = 1;
      break;
    case 'n':
      fl->n = 1;
      break;
    case 'v':
      fl->v = 1;
      break;
    default:
      fprintf(stderr, "ERROR (read_flags): invalid option '%c'\n", arg[i]);
      error = 1;
      break;
    }
  }
  return error;
}

int is_noprint(int ch) {
  int res = 0;
  if (ch != '\t' && ch != '\n' && ((ch < 32 && ch >= 0) || ch >= 127))
    res = 1;
  return res;
}

void add_symbol(Flags fl, int ch, char **str, int *p_i) {
  if (fl.t && ch == '\t') {
    (*str)[(*p_i)++] = '^';
    (*str)[(*p_i)++] = 'I';
  } else if (fl.v && is_noprint(ch)) {
    if ((ch >= 0 && ch < 9) || (ch > 10 && ch < 32) || ch == 127) {
      (*str)[(*p_i)++] = '^';
      (*str)[(*p_i)++] = (ch + 64) % 128;
    } else if (ch >= 128) {
      (*str)[(*p_i)++] = 'M';
      (*str)[(*p_i)++] = '-';
      if (ch <= 159 || ch == 255) {
        (*str)[(*p_i)++] = '^';
        (*str)[(*p_i)++] = (ch + 64) % 128;
      } else if (ch >= 160 && ch <= 254) {
        (*str)[(*p_i)++] = (ch + 128) % 128;
      }
    }
  } else {
    (*str)[(*p_i)++] = ch;
  }
  //(*str)[*p_i + 1] = '\0';
}

int read_str(Flags fl, FILE *file, char **str, int *str_len) {
  int result = 0;
  int s = 1; // s*STR_BUFFER - size of string
  int ch = getc(file);
  for (; ch != '\n' && ch != EOF && result == 0;) {
    add_symbol(fl, ch, str, str_len);
    ch = getc(file);
    if ((STR_BUFFER * s) - strlen(*str) < 16) {
      s++;
      result = resize_str(str, s);
    }
  }
  if (result == 0 && ch == EOF)
    result = -1;
  return result;
}

void print_str(Flags fl, char *strbuffer, int str_len, int file_state) {
  // count overall str number, empty string and flag to empty last string in
  // file
  static int s_count, s_empty, nonempty_last_string;
  s_empty = (str_len == 0) ? s_empty + 1 : 0;
  if (!(fl.s && s_empty > 1)) {
    if (!nonempty_last_string &&
        ((fl.b && s_empty == 0) ||
         (fl.n && !fl.b && !(s_empty > 0 && file_state == -1)))) {
      s_count++;
      printf("%6d\t", s_count);
    }
    for (int i = 0; i < str_len; i++) {
      putchar(strbuffer[i]);
    }
    nonempty_last_string = (file_state == -1 && !s_empty) ? 1 : 0;
    if (file_state != -1) {
      if (fl.e)
        printf("$");
      printf("\n");
    }
  }
}

int cat_file(Flags fl, char *filename) {
  int error = 0; // error flag
  FILE *file = fopen(filename, "r");
  fseek(file, 0L, SEEK_SET);
  int file_state = 0;
  int str_len = 0;
  while (error == 0 && file_state == 0) {
    char *strbuffer = calloc(STR_BUFFER, sizeof(char));
    if (strbuffer != NULL) {
      file_state = read_str(fl, file, &strbuffer, &str_len);
      if (file_state != 1) {
        print_str(fl, strbuffer, str_len, file_state);
        str_len = 0;
      } else
        error = 1;
      free(strbuffer);
    } else {
      fprintf(stderr, "ERROR(read_str): Memory allocation error\n");
      error = 1;
    }
  }
  fclose(file);
  return error;
}

int check_file(const char *filename) // 1 if not exist, -1 if empty, 0 is ok
{
  char res = 0;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
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