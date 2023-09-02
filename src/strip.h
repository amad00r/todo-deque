#ifndef STRIP_H
#define STRIP_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>

char *strip(char *str) {
    const int STR_SIZE = strlen(str);
    int new_str_size = STR_SIZE;
    char *new_str = str;

    while (isspace(*new_str)) {
        ++new_str;
        --new_str_size;
    }

    char c;
    while (new_str_size > 0 && (isspace(c = new_str[new_str_size - 1]) || c == EOF)) --new_str_size;

    char *stripped_str = malloc(sizeof(char)*(new_str_size + 1));
    if (stripped_str == NULL) return NULL;
    strncpy(stripped_str, new_str, new_str_size);
    stripped_str[new_str_size] = '\0';

    free(str);
    return stripped_str;
}

#endif