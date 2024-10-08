#version 460 core

layout (location = 0) in vec3 aPos; // Position array
layout(location = 1) in vec3 aNormal; // Normal array

out DATA
{
    vec4 outlineColor;
    mat4 projection;
} data_out;


// NEVER DECLARE UNIFORMS IF THEY GO UNUSED

// Viewport uniforms
uniform mat4 camMatrix; // Camera view matrix
uniform mat4 model; // Model data for object
uniform mat4 translation; // Translation matrix
uniform mat4 rotation; // Rotation matrix
uniform mat4 scale; // Scale matrix

uniform mat4 gTranslation; // Global Translation matrix
uniform mat4 gRotation; // Global Rotation matrix
uniform mat4 gScale; // Global Scale matrix

uniform float outlining; // Outline thickness
uniform vec4 color; // Outline color

void main(){
    gl_Position = gTranslation * gRotation * gScale * (model * translation * rotation * scale * vec4(aPos + aNormal * (outlining * 0.08), 1.0f));

    data_out.outlineColor = color;
    data_out.projection = camMatrix;
}