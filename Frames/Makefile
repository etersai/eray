CC = gcc                           #debug build
CFLAGS = -I./include -Wall -Wextra -ggdb -DDEBUG -O0
LDFLAGS = -L./lib -lraylib -lm -lpthread -ldl -lrt -lX11

game: src/main.c
	$(CC) $(CFLAGS) src/main.c -o game $(LDFLAGS)

test: src/main2.c
	$(CC) $(CFLAGS) src/main2.c -o test $(LDFLAGS)

imgui: src/main3.c
	$(CC) $(CFLAGS) src/main3.c -o imgui $(LDFLAGS)

clean:
	rm -f game test imgui
