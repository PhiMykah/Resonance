#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (triangles) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (triangle_strip, max_vertices = 3) out;

out vec3 Color;
out vec2 texCoord;

in DATA
{
    vec3 Position;
    vec3 Color;
    mat4 projection;
    vec2 texCoord;
} data_in[];

void main(){
    for (int i = 0; i < 3; i++) {
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        Color = data_in[i].Color;
        texCoord = data_in[i].texCoord;
        // // Set point size
        // gl_PointSize = pointSize;
        // Call EmitVertex() when done with operating on vertex
        EmitVertex();
    }

    // Call EndPrimitive when done with all vertices
    EndPrimitive();
}
