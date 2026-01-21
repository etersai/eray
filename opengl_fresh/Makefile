main: generate_bin
	cc -Wall -Wextra src/main.c src/platform.c src/glad.c -I./include -L./lib -o bin/prog \
	   -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 

generate_bin:
	@rm -rf bin
	@mkdir -p bin/assets
	@cp assets/* bin/assets/

clean:
	@rm -rf bin
