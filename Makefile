CC = clang #x86_64-w64-mingw32-gcc-win32
DEPS = src/log.h src/serialized_tasks_deque.h src/error_handling.h src/strip.h
OBJ = bin/tododq.o

bin/%.o: src/%.c $(DEPS)
	mkdir -p bin
	$(CC) -O2 -Wall -Wextra -ggdb -c -o $@ $<

bin/tododq: $(OBJ)
	$(CC) -O2 -Wall -Wextra -ggdb -o $@ $^

clear:
	rm bin/*
