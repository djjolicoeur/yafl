CC=cc
CFLAGS=-std=c99 -Wall
EXECUTABLE=bin/yafl
LINKER_FLAGS=-ledit -lm


all:
	$(CC) $(CCFLAGS) src/yafl.c src/parser.c src/reader.c src/builtins.c src/lenv.c src/lval.c src/mpc/mpc.c $(LINKER_FLAGS) -o $(EXECUTABLE)

clean:
	rm bin/yafl
