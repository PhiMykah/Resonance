#version 460 core

out vec4 FragColor;

// Primary point color
uniform vec4 pointColor;

void main()
{
    FragColor = pointColor;
}