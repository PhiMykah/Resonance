#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

void main()
{
    
    // Pos stores the final transformed coordinates (screen space)
    vec3 pos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0));

    gl_Position = camMatrix * vec4(pos, 1.0);
    
    // Flip z axis value since cubemaps are left handed coordinates
    // whilst OpenGL uses right handed coordinates
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}