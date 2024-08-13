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
        VAO vao;

        Mesh();

        Mesh(Vertices& vertices, Indices& indices, Textures& textures);

        void Draw(Shader& shader, Camera& camera);
    protected:
        void initMesh(Vertices& vertices, Indices& indices, Textures& textures);
};

#endif // !MESH_CLASS_H