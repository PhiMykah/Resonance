#version 460 core
layout(location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aNormal; // Normal array
layout(location = 2) in vec3 aColor; // Color arrays
layout(location = 3) in vec2 aTex; // Texture array

out DATA
{
    vec3 Normal; // Outputs the normal vectors of the object
    mat4 projection; // Include projection matrix since it is applied after geometry shader
} data_out;

// NEVER DECLARE UNIFORMS IF THEY GO UNUSED
uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

uniform mat4 gTranslation; // Global Translation matrix
uniform mat4 gRotation; // Global Rotation matrix
uniform mat4 gScale; // Global Scale matrix

void main()
{
    // Calculate current position   
                    // Global transform                 //                      Local Transform
    gl_Position = gTranslation * gRotation * gScale * (model * translation * rotation * scale * vec4(aPos, 1.0));

    // Assigns the normal vectors from the vertex data to "Normal"
    data_out.Normal = aNormal;

    data_out.projection = camMatrix;
}
