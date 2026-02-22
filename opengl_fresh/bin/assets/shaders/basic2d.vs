#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;

uniform mat4 model, view, projection;

out vec4 pass_color;
out vec2 pass_uv;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   pass_uv = aUV;
   pass_color = aColor;
}

//w = 1.0 - it's a point in space, gets affected by translation
//w = 0.0 - it's a direction/vector, translation has no effect on it
