EXES = display
CC = clang
CFLAGS = -std=c2x -Weverything -Wno-c++98-compat -Wno-padded -Wno-unused-macros -D_POSIX_SOURCE

.PHONY: all clean

all: $(EXES)

clean:
	-rm $(wildcard *.o) $(EXES)
