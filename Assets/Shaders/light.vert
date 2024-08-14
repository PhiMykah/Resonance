#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

void main()
{
    gl_Position = camMatrix * model * translation * -rotation * scale * vec4(aPos, 1.0f);
}