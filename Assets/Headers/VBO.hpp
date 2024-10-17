#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texUV;
};

struct PosVertex
{
    glm::vec3 position;
};

struct LineVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

struct TextVertex
{
    glm::vec2 position;
    glm::vec2 texUV;
};

/*
### Vertex Buffer Object (VBO)
Class for containing and handling OpenGL vertex buffer
*/
template <typename Vert> class VBO
{
    public:
        // Reference ID for VBO
        GLuint ID;

        // VBO constructor for storing vertex data and VBO information
        VBO(std::vector<Vert>& vertices, GLenum usage = GL_STATIC_DRAW);

        VBO();

        void BufferData(unsigned int size, GLenum usage = GL_STATIC_DRAW);

        // Bind VBO to binding point
        void Bind();

        // Unbind VBO from binding point
        void Unbind();

        // Delete VBO
        void Delete();
};

#endif // !VBO_CLASS_H