CFLAGS = -g -Wall
LDFLAGS =

hello: main.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
clean:
	rm -rf hello

.PHONY: clean