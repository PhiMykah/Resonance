#version 460 core

out vec4 FragColor;

in vec4 outlineColor;

void main() {
    // Use basic color for the stencil outline
    FragColor = outlineColor;
}