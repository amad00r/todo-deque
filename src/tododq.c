#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "serialized_tasks_deque.h"
#include "strip.h"
#include "log.h"
#include "error_handling.h"


int is_help_option(const char *str) {
    return !strcmp(str, "-h") || !strcmp(str, "--help");
}

int is_file_option(const char *str) {
    return !strcmp(str, "-f") || !strcmp(str, "--file");
}

int main(int argc, char **argv) {
    char *arg1 = argv[1];
    int cmp;
    
    if ((cmp = argc == 1) || (argc == 3 && is_file_option(arg1))) {
        int fd = check(sdq_rdonly_open(cmp ? NULL : argv[2]));

        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));

        if (sdq.size) {
            puts(sdq.buf);
            sdq_free(&sdq);
            check(close(fd)); 
            return EXIT_SUCCESS;
        }
        
        info("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");
    }

    else if (is_help_option(arg1)) {
        if (argc != 2) fail("unexpected number of arguments");
        usage();
    }

    else if ((cmp = !strcmp(arg1, "pushf")) || !strcmp(arg1, "pushb")) {
        // TODO: add possibility to select a specific serialized deque file
        int fd = check(sdq_rdwr_open(NULL));

        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));

        if (argc == 2) {
            size_t size;
            char *task = NULL;
            check(getdelim(&task, &size, EOF, stdin));
            task = check_null(strip(task));
            check(cmp ? sdq_push_front(&sdq, task) : sdq_push_back(&sdq, task));
            free(task);
        }
        else
            for (int i = 2; i < argc; ++i)
                check(sdq_push_front(&sdq, argv[i]));

        check(sdq_write(&sdq, fd));

        check(close(fd));
        sdq_free(&sdq);
    }

    else if (!strcmp(arg1, "complete")) {
        char *sdq_path;
        if (argc == 2) sdq_path = NULL;
        else if (argc == 4 && is_file_option(argv[2])) sdq_path = argv[3];
        else fail("unexpected arguments");

        int fd = check(sdq_rdwr_open(sdq_path));
 
        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));
        if (sdq.size) {
            check(sdq_pop(&sdq));
            check(sdq_clear(sdq_path));
            check(sdq_write(&sdq, fd));
            check(close(fd));
            sdq_free(&sdq);
            return EXIT_SUCCESS;
        }

        fail("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");       
    }

    else if (!strcmp(arg1, "clear")) {
        // TODO: add -y option to slip confirmation
        if ((cmp = argc == 2) || (argc == 4 && is_file_option(argv[2]))) {
            size_t size;
            char *confirmation = NULL;
            fputs("Are you sure you want to clear the specified serialized todo deque? [y/n]: ", stdout);
            check(getline(&confirmation, &size, stdin));
            if (strcmp(check_null(strip(confirmation)), "y")) return EXIT_FAILURE;

            check(sdq_clear(cmp ? NULL : argv[3]));
        }
        else fail("unexpected arguments");
    }

    else if (!strcmp(arg1, "slide")) {
        int fd;
        if (argc == 2)
            fd = check(sdq_rdwr_open(NULL));
        else if (argc == 4 && is_file_option(argv[2]))
            fd = check(sdq_rdwr_open(argv[3]));
        else 
            fail("unexpected arguments");

        SerializedDeque sdq;
        check(sdq_read(&sdq, fd));
        if (sdq.size) {
            check(sdq_slide(&sdq));
            check(sdq_write(&sdq, fd));
            check(close(fd));
            sdq_free(&sdq);
            return EXIT_SUCCESS;
        }

        fail("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");
    }

    else fail("unexpected action\n"
              "use `tododq -h to list all the possible actions`");

    return EXIT_SUCCESS;
}
