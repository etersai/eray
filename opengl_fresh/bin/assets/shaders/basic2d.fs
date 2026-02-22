#version 460 core

uniform sampler2D tex;
uniform bool use_tex;
uniform vec4 color;

in vec4 pass_color;
in vec2 pass_uv;

out vec4 frag_color;

void main()
{
   frag_color = (use_tex ? texture(tex, pass_uv) : vec4(1.0)) * color;
}
