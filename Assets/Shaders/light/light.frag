#version 460 core

out vec4 FragColor;

// Light source color
uniform vec4 lightColor;

void main()
{
    FragColor = lightColor;
}