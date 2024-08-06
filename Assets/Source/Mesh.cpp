#include "Mesh.h"

Mesh::Mesh(Vertices& vertices, Indices& indices, Textures& textures){
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    // Vertex array object MUST be created before vertex buffer object
    // Vertex buffer is a different kind of buffer than the front and back buffer
    
    // Bind Vertex Array Object (VAO)
    vao.Bind();

    // Vertex Buffer Object (VBO)
    VBO vbo(vertices);
    // Index Buffer Object (EBO)
    EBO ebo(indices);

    // Link vbo layouts to corresponding vao
    // Position Coordinate layout (layout 0)
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    // Normals (layout 1) 
    vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
    // Color layout (layout 2)
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));
    // Texture coordinate layout (layout 3)
    vao.LinkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)(9 * sizeof(float)));
    
    // Unbind vao, vbo, and ebo to avoid further modifications
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera){

    // Activate shader and bind vao to shader
    shader.Activate();
    vao.Bind();

    // Initialize diffuse texture and specular texture count
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    // For loop iterates over all textures, and categorizes each texture
    // as diffuse or specular
    for (unsigned int i = 0; i < textures.size(); i++) {
        std::string num;
        std::string type = textures[i].type;
        // Increment diffuse texture count if texture is diffuse
        if (type == "diffuse") {
            num = std::to_string(numDiffuse++); // Increment happens after line is run
        }
        // Increment specular texture count if texture is specular
        else if (type == "specular") {
            num = std::to_string(numSpecular++); // Increment happens after line is run
        }

        // Add texture to texUnit based on number given
        textures[i].texUnit(shader, (type + num).c_str(), i);
        // Bind the texture to shader
        textures[i].Bind();
    }
    // Send camera position to shader and perform camera matrix calculations
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}