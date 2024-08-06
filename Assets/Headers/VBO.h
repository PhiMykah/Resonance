#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

/*
### Vertex Buffer Object (VBO)
Class for containing and handling OpenGL vertex buffer
*/
class VBO
{
    public:
        // Reference ID for VBO
        GLuint ID;

        // VBO constructor for storing vertex data and VBO information
        VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

        // Bind VBO to binding point
        void Bind();

        // Unbind VBO from binding point
        void Unbind();

        // Delete VBO
        void Delete();
};

#endif // !VBO_CLASS_H