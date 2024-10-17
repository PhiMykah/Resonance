#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (points) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (points, max_vertices = 1) out;

out vec3 Color;

in DATA
{
    vec3 Color;
    mat4 projection;
} data_in[];

uniform float pointSize; // Float defining size of points

void main(){
    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    Color = data_in[0].Color;
    
    // Set point size
    gl_PointSize = pointSize;

    // Call EmitVertex() when done with operating on vertex
    EmitVertex();
    // Call EndPrimitive when done with all vertices
    EndPrimitive();
}
