#include <stdlib.h>
#include <stdio.h>

#include "serialized_tasks_deque.h"
#include "strip.h"
#include "log.h"
#include "error_handling.h"


int is_help_option(const char *str) {
    return !strcmp(str, "-h") || !strcmp(str, "--help");
}

int main(int argc, char **argv) {
    int cmp;
    
    if (argc == 1) {
        FILE *f = check_null(sdq_open());

        SerializedDeque sdq;
        check(sdq_read(&sdq, f));

        if (sdq.size) {
            puts(sdq.buf);
            sdq_free(&sdq);
            check(fclose(f));
            return EXIT_SUCCESS;
        }
        
        info("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");
    }

    else if (is_help_option(argv[1])) {
        if (argc != 2) fail("unexpected number of arguments");
        usage();
    }

    else if ((cmp = !strcmp(argv[1], "pushf")) || !strcmp(argv[1], "pushb")) {
        FILE *f = check_null(sdq_open());

        SerializedDeque sdq;
        check(sdq_read(&sdq, f));
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

        check(sdq_write(&sdq, f));

        check(fclose(f));
        sdq_free(&sdq);
    }

    else if (!strcmp(argv[1], "complete")) {
        if (argc != 2) fail("unexpected arguments");
        FILE *f = check_null(sdq_open());
 
        SerializedDeque sdq;
        check(sdq_read(&sdq, f));
        if (sdq.size) {
            check(sdq_pop(&sdq));
            check(sdq_clear());
            check(sdq_write(&sdq, f));
            check(fclose(f));
            sdq_free(&sdq);
            return EXIT_SUCCESS;
        }

        fail("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");       
    }

    else if (!strcmp(argv[1], "clear")) {
        if (argc == 2) {
            size_t size;
            char *confirmation = NULL;
            fputs("Are you sure you want to clear the specified serialized todo deque? [y/n]: ", stdout);
            check(getline(&confirmation, &size, stdin));
            if (strcmp(check_null(strip(confirmation)), "y")) return EXIT_FAILURE;

            check(sdq_clear());
        }
        else fail("unexpected arguments");
    }

    else if (!strcmp(argv[1], "slide")) {
        if (argc != 2) fail("unexpected arguments");
        FILE *f = check_null(sdq_open());

        SerializedDeque sdq;
        check(sdq_read(&sdq, f));
        if (sdq.size) {
            check(sdq_slide(&sdq));
            check(sdq_write(&sdq, f));
            check(fclose(f));
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
