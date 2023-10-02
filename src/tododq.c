#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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
            puts(sdq.buf);
            sdq_free(&sdq);
            check(close(fd)); 
            return EXIT_SUCCESS;
        }
        
        info("todo deque is empty :)\n"
             "use `tododq -h` to learn how to add a task");
    }
    else {
        char *action_arg = argv[1];

        if (!strcmp(action_arg, "-h") || !strcmp(action_arg, "--help")) {
            if (argc != 2) fail("unexpected number of arguments");
            usage();
        }

        else if (!strcmp(action_arg, "pushf")) {
            if (argc != 2) fail("unexpected number of arguments");

            int fd = check(sdq_rdwr_open());

            size_t size;
            char *task = NULL;
            check(getdelim(&task, &size, EOF, stdin));
            task = check_null(strip(task));

            SerializedDeque sdq;
            check(sdq_read(&sdq, fd));
            check(sdq_push_front(&sdq, task));
            check(sdq_write(&sdq, fd));

            check(close(fd));
            free(task);
            sdq_free(&sdq);
        }

        else if (!strcmp(action_arg, "pushb")) {
            if (argc != 2) fail("unexpected number of arguments");

            int fd = check(sdq_rdwr_open());

            size_t size;
            char *task = NULL;
            check(getdelim(&task, &size, EOF, stdin));
            task = check_null(strip(task));

            SerializedDeque sdq;
            check(sdq_read(&sdq, fd));
            check(sdq_push_back(&sdq, task));
            check(sdq_write(&sdq, fd));

            check(close(fd));
            free(task);
            sdq_free(&sdq);
        }

        else if (!strcmp(action_arg, "complete")) {
            if (argc != 2) fail("unexpected number of arguments");

            int fd = check(sdq_rdwr_open());

            SerializedDeque sdq;
            check(sdq_read(&sdq, fd));
            if (sdq.size) {
                check(sdq_pop(&sdq));
                check(sdq_clear());
                check(sdq_write(&sdq, fd));
                check(close(fd));
                sdq_free(&sdq);
                return EXIT_SUCCESS;
            }

            fail("todo deque is empty :)\n"
                "use `tododq -h` to learn how to add a task");       
        }

        else if (!strcmp(action_arg, "clear")) {
            if (argc != 2) fail("unexpected number of arguments");

            check(sdq_clear());
        }

        else if (!strcmp(action_arg, "slide")) {
            if (argc != 2) fail("unexpected number of arguments");

            assert(0);
        }

        else fail("unexpected action\n"
                  "use `tododq -h to list all the possible actions`");
    }
}
