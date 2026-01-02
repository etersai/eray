CC = gcc
CFLAGS = -I./include -g -Wall -Wextra 
LDFLAGS = -L./lib -lraylib -lm -lpthread -ldl -lrt -lX11

game: src/main.c
	$(CC) $(CFLAGS) \
	src/main.c \
	-o game \
	$(LDFLAGS)

clean:
	rm -f game
