#version 460 core
layout(location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aColor; // Color array
layout(location = 2) in vec2 aTex; // Texture array
layout(location = 3) in vec3 aNormal; // Normal array

// Output color for fragment shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;
// Outputs the normal vectors of the object
out vec3 Normal;
// Output the current position fo the object
out vec3 currPos; 

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform float size; // Object size
uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
void main()
{
    currPos = vec3(model * vec4(size * aPos, 1.0));

    gl_Position = camMatrix * vec4(currPos, 1.0);
    // Assign colors from vertex data to color
    color = aColor;
    // Assigns the texture coordinates from the vertex data to "texCoord"
    texCoord = aTex;
    // Assigns the normal vectors from the vertex data to "Normal"
    Normal = aNormal;
}
