#version 460 core
layout (location = 0) in vec3 aPos; // Position array
layout (location = 1) in vec3 aColor; // Color array
layout (location = 2) in vec2 aTex; // Texture array

// Output color for fragment shader
out vec3 color;

out vec2 texCoord;

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform float size;

// Transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(size * aPos, 1.0);
    // Assign colors from vertex data to color
    color = aColor;
    texCoord = aTex;
}