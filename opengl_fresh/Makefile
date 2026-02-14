main: generate_bin # d_posix for elog_ timing.
	cc -std=c11 -D_POSIX_C_SOURCE=199309L -Wall -Wextra -ggdb src/main.c src/font.c src/r_shader.c src/r_opengl.c src/common/str.c src/obj_loader.c src/r_main.c src/r_camerka.c src/platform.c src/glad.c -I./include -L./lib -o bin/prog \
	   -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 
# -Werror, -pedantic
generate_bin:
	@rm -rf bin
	@mkdir -p bin/assets/skybox
	@mkdir -p bin/assets/models 
	@mkdir -p bin/assets/textures
	@cp -r assets/* bin/assets/

clean:
	@rm -rf bin
