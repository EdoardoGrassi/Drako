#version 440

#if !defined(VULKAN)
#error Required support for GL_KHR_vulkan_glsl extension
#endif

layout(location = 0, binding = 0) in InputInterfaceBlock
{
    vec3 position;
    vec3 normal;
};

layout(location = 0) out OutputInterfaceBlock
{
    vec3 color;
};

void gooch(const in vec3 c_surface, out vec3 color)
{
    const vec3 c_cool = (0, 0, 0.55) + 0.25 * c_surface;
    const vec3 c_warm = (0.3, 0.3, 0) + 0.25 * c_surface;
    const vec3 c_high = (2, 2, 2);

    // unlit component
    const vec3 f_unlit = 0.5 * c_cool;

    // lit component
    const float r = reflect(view, normal);
    const float specular = clamp(100 * dot(r, v) - 97, 0.0, 1.0);
    const vec3 f_lit = mix(c_warm, c_high, specular);

    color = f_unlit + f_lit;
}