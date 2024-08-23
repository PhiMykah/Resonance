#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // Pos stores the final transformed coordinates (screen space)
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    // Setting the z value to pos.w or 1.0f ensures the skybox 
    // is always at the limit of the depth buffer
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    // Flip z axis value since cubemaps are left handed coordinates
    // whilst OpenGL uses right handed coordinates
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}