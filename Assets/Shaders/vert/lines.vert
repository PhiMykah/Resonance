#version 460 core
layout(location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aColor; // Color arrays

out DATA
{
    mat4 projection;
    vec3 color; // Output color for fragment shader
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
                    // Global transform                 //                      Local Transform
    gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0);

    // Assign colors from vertex data to color
    data_out.color = aColor;

    data_out.projection = camMatrix;
}
