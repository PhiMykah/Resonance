#version 460 core
layout(location = 0) in vec3 aPos; // Position array

out DATA
{
    mat4 projection; // Include projection matrix since it is applied after geometry shader
} data_out;

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

void main()
{
    // Calculate current position   
    gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0);

    data_out.projection = camMatrix;
}