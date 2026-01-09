CC = gcc                           #debug build
CFLAGS = -I./include -Wall -Wextra -ggdb -DDEBUG -O0
LDFLAGS = -L./lib -lraylib -lm -lpthread -ldl -lrt -lX11

game: src/main.c
	$(CC) $(CFLAGS) \
	src/main.c \
	-o game \
	$(LDFLAGS)

clean:
	rm -f game
