#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (triangles) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (triangle_strip, max_vertices = 3) out;

in DATA
{
    mat4 projection;
} data_in[];

void main()
{
    // Vertex #1 (index 0)
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    EmitVertex();

    // Vertex #2 (index 1)
    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    EmitVertex();

    // Vertex #3 (index 2)
    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}