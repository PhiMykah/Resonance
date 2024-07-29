#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// Output color for fragment shader
out vec3 color;

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform float size;

void main()
{
    gl_Position = vec4(size * aPos.x, size * aPos.y, size * aPos.z, 1.0);
    // Assign colors from vertex data to color
    color = aColor;
}