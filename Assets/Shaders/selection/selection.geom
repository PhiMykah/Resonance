#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (triangles) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (triangle_strip, max_vertices = 3) out;

// Position is not necessary to import since
// it is included in the default gl_in structure
in DATA
{
    mat4 projection;
} data_in[];    

void main()
{
    for (int i = 0; i < 3; i++) {
        // Vertex #1 (index 0)
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        // Call EmitVertex() when done with operating on vertex
        EmitVertex();
    }

    // Call EndPrimitive when done with all vertices
    EndPrimitive();
}