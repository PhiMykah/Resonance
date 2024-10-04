#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (points) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (points, max_vertices = 1) out;

// Pass on vertex shader variables to fragment shader
// since geometry shader is inbetween vertex shader
// and fragment shader in the pipeline
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 currPos;

// Position is not necessary to import since
// it is included in the default gl_in structure
in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    vec3 currPos;
    mat4 projection;
} data_in[];

uniform float pointSize; // Float defining size of points

void defaultGeometry()
{
    for (int i = 0; i < 3; i++) {
        // Vertex #1 (index 0)
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        Normal = data_in[i].Normal;
        color = data_in[i].color;
        texCoord = data_in[i].texCoord;
        currPos = data_in[i].currPos;
        // Set point size
        gl_PointSize = pointSize;
        // Call EmitVertex() when done with operating on vertex
        EmitVertex();
    }

    // Call EndPrimitive when done with all vertices
    EndPrimitive();
}

void main(){
    defaultGeometry();
}