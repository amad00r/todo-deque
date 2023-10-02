#ifndef LOG_H
#define LOG_H

#include <stdlib.h>
#include <stdio.h>

void fail(const char *msg) {
    fputs("[ERROR] ", stderr);
    fputs(msg, stderr);
    putc('\n', stderr);
    exit(EXIT_FAILURE);
}

void info(const char *msg) {
    fputs("[INFO] ", stdout);
    puts(msg);
}

void usage(void) {
    puts(
        "Usage:\n"
        "    tododq\n"
        "    tododq pushf [TASK1, TASK2, ...]\n"
        "    tododq pushb [TASK1, TASK2, ...]\n"
        "    tododq complete\n"
        "    tododq clear\n"
        "    tododq slide"
    );
}

#endif