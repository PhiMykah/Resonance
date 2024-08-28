#version 330 core

// Transform triangles into 6 lines 
// (Creating new vertices)
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in DATA
{
    vec3 Normal;
    mat4 projection;
} data_in[];

uniform float hairLength; // Length that the normals extend

void main()
{
    // ***** Vertex 1 ******
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    EmitVertex();
    // Vertex #1's normal vertex
    gl_Position = data_in[0].projection * (gl_in[0].gl_Position + hairLength * vec4(data_in[0].Normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    // ***** Vertex 2 ******
    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    EmitVertex();
    // Vertex #2's normal vertex
    gl_Position = data_in[1].projection * (gl_in[1].gl_Position + hairLength * vec4(data_in[1].Normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    // ***** Vertex 3 ******
    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    EmitVertex();
    // Vertex #3's normal vertex
    gl_Position = data_in[2].projection * (gl_in[2].gl_Position + hairLength * vec4(data_in[2].Normal, 0.0f));
    EmitVertex();
    EndPrimitive();
}