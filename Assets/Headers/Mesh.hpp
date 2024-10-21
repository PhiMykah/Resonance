#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include "VAO.hpp"
#include "EBO.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "Constants.hpp"

#define Vertices std::vector<Vertex>
#define PosVertices std::vector<PosVertex>
#define Indices std::vector<GLuint>
#define Textures std::vector<Texture>

/*
Mesh Object which holds information such as the verts, indices, and textures
*/
class Mesh
{
    public:
        Vertices vertices;
        Indices indices;
        Textures textures;
        PosVertices posVertices;

        GLenum primative = GL_TRIANGLES;
        VAO<Vertex> vao;

        // Empty constructor for Mesh object
        Mesh();

        // Initialize mesh given vertices, indices, and textures
        Mesh(Vertices& vertices, Indices& indices, Textures& textures);

        // Set new primative type
        void SetPrimative(GLenum primative);
        
        // Draw mesh to given camera viewport with given shader
        // Additionally, modify mesh with given translation, rotation, and scale
        void Draw
        (
            Shader& shader,
            Camera& camera,
            glm::mat4 matrix = MAT_IDENTITY,
            glm::vec3 translation = ZEROS,
            glm::quat rotation = QUAT_IDENTITY,
            glm::vec3 scale = ONES,
            glm::vec3 globalTranslation = ZEROS,
            glm::quat globalRotation = QUAT_IDENTITY,
            glm::vec3 globalScale = ONES
        );
        
        GLuint ID;
        glm::vec3 pos = ZEROS;
    protected:
        // Internal mesh initialization function used by Mesh and its children
        void initMesh(Vertices& vertices, Indices& indices, Textures& textures);
};

#endif // !MESH_CLASS_H