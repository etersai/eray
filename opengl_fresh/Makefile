main: generate_bin src/main.c
	cc -Wall -Wextra src/main.c src/glad.c -I./include -L./lib \
	-o bin/prog -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 

generate_bin:
	@rm -rf bin
	@mkdir bin

clean:
	@rm -rf bin
