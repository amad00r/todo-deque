#ifndef SERIALIZED_TASKS_DEQUE_H
#define SERIALIZED_TASKS_DEQUE_H

#include <stdlib.h>
#include <string.h>

#include "log.h"

typedef struct SerializedDeque {
    char *buf;
    size_t size;
} SerializedDeque;

void sdq_init(SerializedDeque *sdq, char *buf, size_t size) {
    sdq->buf = buf;
    sdq->size = size;
}

void sdq_free(SerializedDeque *sdq) {
    free(sdq->buf);
}

char *sdq_get_path(void) {
    const char *HOME = getenv("HOME");
    const int HOME_SIZE = strlen(HOME);
    const int FILENAME_SIZE = strlen("/.tododq");

    char *path = malloc(sizeof(char)*(HOME_SIZE + FILENAME_SIZE + 1));
    if (path == NULL) return NULL;

    strncpy(path, HOME, HOME_SIZE + 1);
    strncat(path, "/.tododq", FILENAME_SIZE + 1);

    return path;
}

FILE *sdq_open(void) {
    char *sdq_path = sdq_get_path();
    if (sdq_path == NULL) return NULL;
    
    FILE *f = fopen(sdq_path, "r+");

    free(sdq_path);
    return f;
}

int sdq_clear(void) {
    char *sdq_path = sdq_get_path();
    if (sdq_path == NULL) return -1;

    FILE *f = fopen(sdq_path, "w");
    if (f == NULL) return -1;

    free(sdq_path);
    return fclose(f);
}

size_t sdq_read(SerializedDeque *sdq, FILE *f) {
    if (fseek(f, 0L, SEEK_END) < 0) return -1;
    long nbytes = ftell(f);
    rewind(f);
    if (nbytes < 0) return -1;

    if (nbytes) {
        sdq->size = nbytes;
        if ((sdq->buf = malloc(sizeof(char)*nbytes)) == NULL) return -1;
        size_t r = fread(sdq->buf, sizeof(char), nbytes, f);
        rewind(f);
        return r;
    }
    else {
        sdq->buf = NULL;
        return sdq->size = 0;
    }
}

int sdq_write(SerializedDeque *sdq, FILE *f) {
    rewind(f);
    return fwrite(sdq->buf, sizeof(char), sdq->size, f);
}

int sdq_push_front(SerializedDeque *sdq, const char *task) {
    const size_t TASK_SIZE = strlen(task) + 1;

    if (sdq->size) {
        const size_t NEW_BUF_SIZE = sdq->size + TASK_SIZE;

        char *new_buf = malloc(sizeof(char)*NEW_BUF_SIZE);
        if (new_buf == NULL) return -1;

        memcpy(new_buf, task, TASK_SIZE);
        memcpy(new_buf + TASK_SIZE, sdq->buf, sdq->size);

        free(sdq->buf);
        sdq->buf = new_buf;
        sdq->size = NEW_BUF_SIZE;
    }
    else {
        char *new_buf = malloc(sizeof(char)*TASK_SIZE);
        if (new_buf == NULL) return -1;

        memcpy(new_buf, task, TASK_SIZE);

        sdq->buf = new_buf;
        sdq->size = TASK_SIZE;
    }
    
    return 0;
}


int sdq_push_back(SerializedDeque *sdq, const char *task) {
    const int TASK_SIZE = strlen(task) + 1;

    if (sdq->size) {
        const size_t NEW_BUF_SIZE = sdq->size + TASK_SIZE;

        char *new_buf = malloc(sizeof(char)*NEW_BUF_SIZE);
        if (new_buf == NULL) return -1;

        memcpy(new_buf, sdq->buf, sdq->size);
        memcpy(new_buf + sdq->size, task, TASK_SIZE);

        free(sdq->buf);
        sdq->buf = new_buf;
        sdq->size = NEW_BUF_SIZE;
    }
    else {
        char *new_buf = malloc(sizeof(char)*TASK_SIZE);
        if (new_buf == NULL) return -1;

        memcpy(new_buf, task, TASK_SIZE);

        sdq->buf = new_buf;
        sdq->size = TASK_SIZE;
    }

    return 0;
}

int sdq_pop(SerializedDeque *sdq) {
    char *tmp = sdq->buf;
    size_t tmp_size = sdq->size;
    do --tmp_size;
    while (*tmp++ != '\0');

    if (tmp_size > 0) {
        char *new_buf = malloc(sizeof(char)*tmp_size);
        if (new_buf == NULL) return -1;
        memcpy(new_buf, tmp, tmp_size);

        free(sdq->buf);
        sdq->buf = new_buf;
        sdq->size = tmp_size;
    }
    else {
        free(sdq->buf);
        sdq->buf = NULL;
        sdq->size = 0;
    }

    return 0;
}

int sdq_slide(SerializedDeque *sdq) {
    char *tmp = sdq->buf;
    size_t tmp_size = sdq->size;
    do --tmp_size;
    while (*tmp++ != '\0');

    if (tmp_size) {
        char *new_buf = malloc(sizeof(char)*sdq->size);
        if (new_buf == NULL) return -1;
        memcpy(new_buf, tmp, tmp_size);
        memcpy(new_buf + tmp_size, sdq->buf, sdq->size - tmp_size);

        free(sdq->buf);
        sdq->buf = new_buf;
    }

    return 0;
}

#endif
