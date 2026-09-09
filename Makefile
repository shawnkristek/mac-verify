CC ?= clang
CFLAGS ?= -O3 -Wall

membw: membw.c
	$(CC) $(CFLAGS) -o $@ $< -lpthread

clean:
	rm -f membw

.PHONY: clean
