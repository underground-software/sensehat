EXES = full_sensehat_test sensehat_display_test sensehat_joystick_test
CC = clang
CFLAGS = -std=c2x -Weverything -Wno-c++98-compat -Wno-padded -Wno-unused-macros -D_POSIX_SOURCE

.PHONY: all clean

all: $(EXES)

clean:
	-rm $(wildcard *.o) $(EXES)
