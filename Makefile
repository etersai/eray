PROG_NAME = eray
CC = gcc
CFLAGS = -I./include -g -Wall -Wextra 
LDFLAGS = -L./lib -lraylib -lm -lpthread -ldl -lrt -lX11

game: generate_bin src/main.c src/eray_camera.c
	$(CC) $(CFLAGS) \
	src/main.c \
	src/eray_camera.c \
	-o bin/$(PROG_NAME) \
	$(LDFLAGS)

generate_bin:
	@mkdir -p bin

clean:
	@rm -rf bin

.PHONY: generate_bin clean
