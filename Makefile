CFLAGS = -g -Wall
LDFLAGS =

hello: main.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ -lpthread
clean:
	rm -rf hello

.PHONY: clean