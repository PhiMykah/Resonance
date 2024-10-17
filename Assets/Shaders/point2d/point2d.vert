#version 460 core
layout(location = 0) in vec2 pos;       // Position array
layout(location = 1) in vec3 color;     // Color array
uniform mat4 projection;

out DATA
{
    vec3 Color;
    mat4 projection;
} data_out;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    
    data_out.Color = color;
    data_out.projection = projection;
}  