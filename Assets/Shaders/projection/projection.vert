#version 460 core
layout (location = 0) in vec3 aPos;

out DATA
{
    vec3 texCoords;
    mat4 projection;
} data_out;

uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

void main()
{
    
    gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0);

    data_out.projection = camMatrix;

    // Flip z axis value since cubemaps are left handed coordinates
    // whilst OpenGL uses right handed coordinates
    data_out.texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}