#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (lines) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (line_strip, max_vertices = 2) out;

out vec3 color;

in DATA
{
    mat4 projection;
    vec3 color; // Output color for fragment shader
} data_in[];

void main()
{
    // Vertex #1 (index 0)
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    color = data_in[0].color;
    EmitVertex();

    // Vertex #2 (index 1)
    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    color = data_in[1].color;
    EmitVertex();

    EndPrimitive();
}