#ifndef SERIALIZED_TASKS_DEQUE_H
#define SERIALIZED_TASKS_DEQUE_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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
    const int FILENAME_SIZE = strlen("./tododq");

    char *path = malloc(sizeof(char)*(HOME_SIZE + FILENAME_SIZE + 1));
    if (path == NULL) return NULL;

    strncpy(path, HOME, HOME_SIZE + 1);
    strncat(path, "/.tododq", FILENAME_SIZE + 1);

    return path;
}

int sdq_rdonly_open(void) {
    char *filepath = sdq_get_path();
    if (filepath == NULL) return -1;
    int fd = open(filepath, O_RDONLY | O_CREAT, S_IRWXU);
    free(filepath);
    return fd;
}

int sdq_rdwr_open(void) {
    char *filepath = sdq_get_path();
    if (filepath == NULL) return -1;
    int fd = open(filepath, O_RDWR | O_CREAT, S_IRWXU);
    free(filepath);
    return fd;
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
        sdq->size = nbytes + 1;
        if ((sdq->buf = malloc(sizeof(char)*(sdq->size))) == NULL) return -1;
        sdq->buf[nbytes] = EOF;
        return read(fd, sdq->buf, nbytes);
    }
    else {
        sdq->buf = NULL;
        return sdq->size = 0;
    }
}

int sdq_write(SerializedDeque *sdq, int fd) {
    lseek(fd, 0, SEEK_SET);
    return write(fd, sdq->buf, sdq->size - 1);
}

void sdq_print_front(SerializedDeque *sdq) {
    puts(sdq->buf);
    int title_size = strlen(sdq->buf);
    for (int i = 0; i < title_size; ++i) putchar('@');
    putchar('\n');
    puts(sdq->buf + title_size + 1);
}

int sdq_push_front(SerializedDeque *sdq, const char *title, const char *body) {
    const int TITLE_SIZE = strlen(title) + 1;
    const int BODY_SIZE = strlen(body) + 1;

    if (sdq->size) {
        const int NEW_BUF_SIZE = sdq->size + TITLE_SIZE + BODY_SIZE;

        char *new_buf = malloc(sizeof(char)*NEW_BUF_SIZE);
        if (new_buf == NULL) return -1;

        strncpy(new_buf, title, TITLE_SIZE);
        strncpy(new_buf + TITLE_SIZE, body, BODY_SIZE);
        memcpy(new_buf + TITLE_SIZE + BODY_SIZE, sdq->buf, sdq->size);

        free(sdq->buf);
        sdq->buf = new_buf;
        sdq->size = NEW_BUF_SIZE;
    }
    else {
        const int NEW_BUF_SIZE = TITLE_SIZE + BODY_SIZE + 1;

        char *new_buf = malloc(sizeof(char)*NEW_BUF_SIZE);
        if (new_buf == NULL) return -1;

        strncpy(new_buf, title, TITLE_SIZE);
        strncpy(new_buf + TITLE_SIZE, body, BODY_SIZE);
        new_buf[NEW_BUF_SIZE - 1] = EOF;

        sdq->buf = new_buf;
        sdq->size = NEW_BUF_SIZE;
    }
    
    return 0;
}

/* char *get_serialized_deque(int fd, off_t nbytes) {
    char *buf = malloc(sizeof(char)*nbytes);
    if (buf == NULL || read(fd, buf, nbytes) == -1)

    return buf;
} */




/* typedef struct Task {
    char *title;
    char *body;
} Task; */




#endif