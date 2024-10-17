#version 460 core
layout(location = 0) in vec2 pos;       // Position array
layout(location = 1) in vec3 color;     // Color array
layout(location = 2) in vec2 tex;  // texture Coordinates
uniform mat4 projection;

out DATA
{
    vec3 Position;
    vec3 Color;
    mat4 projection;
    vec2 texCoord;
} data_out;

void main()
{
    gl_Position = projection * vec4(pos, 0.0, 1.0);
    
    data_out.Position = vec3(gl_Position);
    data_out.Color = color;
    data_out.projection = projection;
    data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * tex;
}  