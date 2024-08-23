#version 460 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main(){
    // Basic Fragment shader, set frag color to texture based on tex coords
    FragColor = texture(skybox, texCoords);
}