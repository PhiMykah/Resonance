#version 460 core
layout(location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aNormal; // Normal array
layout(location = 2) in vec3 aColor; // Color arrays
layout(location = 3) in vec2 aTex; // Texture array

// Output the current position fo the object
out vec3 currPos; 
// Outputs the normal vectors of the object
out vec3 Normal;
// Output color for fragment shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

uniform float size; // Object size

void main()
{
    // Calculate current position
    //                                match glm to gltf
    currPos = vec3(model * translation * -rotation * scale * vec4(size * aPos, 1.0));
    // Assigns the normal vectors from the vertex data to "Normal"
    Normal = aNormal;
    // Assign colors from vertex data to color
    color = aColor;
    // Assigns the texture coordinates from the vertex data to "texCoord"
    // Rotate texture coords by 90 deg to match gltf standards
    texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

    // Outputs position and coordinates of all vertices
    gl_Position = camMatrix * vec4(currPos, 1.0);
}
