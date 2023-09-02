#include <stdlib.h>
#include <stdio.h>

#include "serialized_tasks_deque.h"
#include "strip.h"
#include "log.h"
#include "error_handling.h"


int main(int argc, char **argv) {
    if (argc == 1) {
        int fd = check(sdq_rdonly_open());

        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));

        if (sdq.size) {
            sdq_print_front(&sdq);
            sdq_free(&sdq);
        }
        else info("todo deque is empty :)\n"
                  "use `tododq -h` to learn how to add a task");

        check(close(fd));
            
        return EXIT_SUCCESS;
    }

    else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        if (argc != 2) fail("unexpected number of arguments");
        usage();
    }

    else if (!strcmp(argv[1], "pushf")) {
        if (argc != 2) fail("unexpected number of arguments");

        int fd = check(sdq_rdwr_open());

        size_t size;

        fputs("(title): ", stdout);
        char *title = NULL;
        check(getline(&title, &size, stdin));

        puts("(body):");
        char *body = NULL;
        check(getdelim(&body, &size, EOF, stdin));

        title = check_null(strip(title));
        body = check_null(strip(body));

        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));
        check(sdq_push_front(&sdq, title, body));
        check(sdq_write(&sdq, fd));

        check(close(fd));
        free(title);
        free(body);
        sdq_free(&sdq);
    }

    else if (!strcmp(argv[1], "pushb")) {
        if (argc != 2) fail("unexpected number of arguments");

        // TODO
    }

    else if (!strcmp(argv[1], "clear")) {
        if (argc != 2) fail("unexpected number of arguments");

        check(sdq_clear());
    }
}