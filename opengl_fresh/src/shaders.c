const char* vertex_shader_src = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 transform;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragment_shader_src = "#version 460 core\n"
    "out vec4 final_color;\n"
    "void main()\n"
    "{\n"
    "   final_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


