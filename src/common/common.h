#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_BUFFER 256  // size for malloc and additional for realloc

int add_str_in_array(char ***files, int *size,
                     const char *str);  // realloc files array, do malloc for
                                        // new filename and put new filename
void free_strarray(char **files, int size);  // free files array

int resize_str(char **str, int s);