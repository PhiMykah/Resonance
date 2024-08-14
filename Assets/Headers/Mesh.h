#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

#define Vertices std::vector<Vertex>
#define Indices std::vector<GLuint>
#define Textures std::vector<Texture>

class Mesh
{
    public:
        Vertices vertices;
        Indices indices;
        Textures textures;
        GLenum primative = GL_TRIANGLES;
        VAO vao;

        Mesh();

        Mesh(Vertices& vertices, Indices& indices, Textures& textures);

        void Draw
        (
            Shader& shader,
            Camera& camera,
            glm::mat4 matrix = glm::mat4(1.0f),
            glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
        );
        
    protected:
        void initMesh(Vertices& vertices, Indices& indices, Textures& textures);
};

#endif // !MESH_CLASS_H