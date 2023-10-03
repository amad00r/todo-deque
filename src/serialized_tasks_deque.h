#ifndef SERIALIZED_TASKS_DEQUE_H
#define SERIALIZED_TASKS_DEQUE_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include "log.h"

typedef struct SerializedDeque {
    char *buf;
    unsigned int size;
} SerializedDeque;

void sdq_init(SerializedDeque *sdq, char *buf, unsigned int size) {
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

int __sdq_open(char *sdq_path, int open_mode) {
    int fd;
    if (sdq_path == NULL) {
        if ((sdq_path = sdq_get_path()) == NULL) return -1;
        fd = open(sdq_path, open_mode | O_CREAT, S_IRWXU);
        free(sdq_path);
        return fd == -1 ? -1 : close(fd);
    }
    return (fd = open(sdq_path, open_mode | O_CREAT, S_IRWXU)) == -1 ? -1 : close(fd);
}

int sdq_rdonly_open(char *sdq_path) {
    return __sdq_open(sdq_path, O_RDONLY);
}

int sdq_rdwr_open(char *sdq_path) {
    return __sdq_open(sdq_path, O_RDWR);
}

int sdq_clear(char *sdq_path) {
    int fd;
    if (sdq_path == NULL) {
        if ((sdq_path = sdq_get_path()) == NULL) return -1;
        fd = open(sdq_path, O_WRONLY | O_TRUNC);
        free(sdq_path);
        return fd == -1 ? -1 : close(fd);
    }
    return (fd = open(sdq_path, O_WRONLY | O_TRUNC)) == -1 ? -1 : close(fd);
}

off_t sdq_count_file_bytes(int fd) {
    off_t nbytes = lseek(fd, 0, SEEK_END);
    if (lseek(fd, 0, SEEK_SET) == -1) return -1;
    return nbytes;
}

int sdq_read(SerializedDeque *sdq, int fd) {
    off_t nbytes = sdq_count_file_bytes(fd);
    if (nbytes == -1) return -1;
    if (nbytes) {
        sdq->size = nbytes;
        if ((sdq->buf = malloc(sizeof(char)*nbytes)) == NULL) return -1;
        return read(fd, sdq->buf, nbytes);
    }
    else {
        sdq->buf = NULL;
        return sdq->size = 0;
    }
}

int sdq_write(SerializedDeque *sdq, int fd) {
    lseek(fd, 0, SEEK_SET);
    return write(fd, sdq->buf, sdq->size);
}

int sdq_push_front(SerializedDeque *sdq, const char *task) {
    const int TASK_SIZE = strlen(task) + 1;

    if (sdq->size) {
        const int NEW_BUF_SIZE = sdq->size + TASK_SIZE;

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
        const int NEW_BUF_SIZE = sdq->size + TASK_SIZE;

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
    int tmp_size = sdq->size;
    do --tmp_size;
    while (*tmp++ != '\0');

    if (tmp_size) {
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
    int tmp_size = sdq->size;
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
