#ifndef LINE_CLASS_H
#define LINE_CLASS_H

#include <string>
#include <vector>
#include "VAO.hpp"
#include "EBO.hpp"
#include "Camera.hpp"

#define LVertices std::vector<LineVertex>
#define Indices std::vector<GLuint>

/*
Line Object which holds information such as the verts, and indices
*/
class Line
{
    public:
        LVertices vertices;
        Indices indices;
        GLenum primative = GL_LINES;
        VAO vao;

        // Empty constructor for Line object
        Line();

        // Initialize mesh given vertices and indices
        Line(LVertices& vertices, Indices& indices);

        // Draw mesh to given camera viewport with given shader
        // Additionally, modify mesh with given translation, rotation, and scale
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
        // Internal mesh initialization function used by Mesh and its children
        void initLines(LVertices& vertices, Indices& indices);
};

#endif // !LINE_CLASS_H