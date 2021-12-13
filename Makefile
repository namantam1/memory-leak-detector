CC=gcc
CGLAGS=-Wall -Werror -Wextra
BINS=example

all: $(BINS)

runtest: test leak.h
	./test

%: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o *.out *.exe $(BINS) test
