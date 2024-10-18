#version 460 core
layout(location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aNormal; // Normal array
layout(location = 2) in vec3 aColor; // Color arrays
layout(location = 3) in vec2 aTex; // Texture array

out DATA
{
    vec3 Normal; // Outputs the normal vectors of the object
    vec3 color; // Output color for fragment shader
    vec2 texCoords; // Outputs the texture coordinates to the fragment shader
    vec3 currPos; // Output the current 3 float position to the fragment shader
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
    // Assign colors from vertex data to color
    data_out.color = aColor;
    // Assigns the texture coordinates from the vertex data to "texCoords"
    // Rotate texture coords by 90 deg to match gltf standards
    data_out.texCoords = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
    // Output projection matrix to perform after geometry shader
    data_out.currPos = vec3(gl_Position);

    data_out.projection = camMatrix;
}
