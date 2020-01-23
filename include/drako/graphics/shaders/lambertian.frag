#version 440

layout(set = 0, binding = 0) uniform point_light_srcs
{
    vec3 point;
};

layout(push_constant) uniform MVPBlock
{
    mat4 mvp;
};

void main()
{
}