main: generate_bin
	cc -std=c11 -Wall -Wextra -ggdb src/main.c src/shader.c src/gl_stuff.c src/camerka.c src/platform.c src/glad.c -I./include -L./lib -o bin/prog \
	   -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 
# -Werror, -pedantic
generate_bin:
	@rm -rf bin
	@mkdir -p bin/assets/skybox
	@mkdir -p bin/assets/models 
	@cp -r assets/* bin/assets/

clean:
	@rm -rf bin
