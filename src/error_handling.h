#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <errno.h>

#include "log.h"

int check(int result_code) {
    if (result_code == -1) fail(strerror(errno));
    return result_code;
}

void *check_null(void *ptr) {
    if (ptr == NULL) fail(strerror(errno));
    return ptr;
}

#endif
