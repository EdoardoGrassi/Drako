#version 440

layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec3 vert_col;
layout(location = 0) out vec3 frag_color;

void main()
{
    frag_color = vert_col;
}