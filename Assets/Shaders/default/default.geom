#version 460 core

// Define primative to receive
// Choose from: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (triangles) in;
// Define primative to output 
// Choose from: points, line_strip, triangle_strip
layout (triangle_strip, max_vertices = 3) out;

// Pass on vertex shader variables to fragment shader
// since geometry shader is inbetween vertex shader
// and fragment shader in the pipeline
out vec3 Normal;
out vec3 color;
out vec2 texCoords;
out vec3 currPos;

// Position is not necessary to import since
// it is included in the default gl_in structure
in DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoords;
    vec3 currPos;
    mat4 projection;
} data_in[];

void defaultGeometry()
{
    for (int i = 0; i < 3; i++) {
        // Vertex #1 (index 0)
        gl_Position = data_in[i].projection * gl_in[i].gl_Position;
        Normal = data_in[i].Normal;
        color = data_in[i].color;
        texCoords = data_in[i].texCoords;
        currPos = data_in[i].currPos;
        // Call EmitVertex() when done with operating on vertex
        EmitVertex();
    }

    // Call EndPrimitive when done with all vertices
    EndPrimitive();
}

void explosionGeometry(){
    vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    vec4 surfaceNormal = vec4(normalize(cross(vector0, vector1)), 0.0f);

    gl_Position = data_in[0].projection * (gl_in[0].gl_Position + surfaceNormal);
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoords = data_in[0].texCoords;
    currPos = data_in[0].currPos;
    EmitVertex();

    gl_Position = data_in[1].projection * (gl_in[1].gl_Position + surfaceNormal);
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoords = data_in[1].texCoords;
    currPos = data_in[1].currPos;
    EmitVertex();

    gl_Position = data_in[2].projection * (gl_in[2].gl_Position + surfaceNormal);
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoords = data_in[2].texCoords;
    currPos = data_in[2].currPos;
    EmitVertex();

    EndPrimitive();
}

void main(){
    defaultGeometry();
}