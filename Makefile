CC = clang
DEPS = src/log.h src/serialized_tasks_deque.h src/error_handling.h src/strip.h
OBJ = bin/tododq.o

bin/%.o: src/%.c $(DEPS)
	$(CC) -Wall -Wextra -ggdb -c -o $@ $<

bin/tododq: $(OBJ)
	$(CC) -Wall -Wextra -ggdb -o $@ $^

clear:
	rm bin/*