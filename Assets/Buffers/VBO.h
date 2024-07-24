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
        GLuint ID;
        VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

        void Bind();
        void Unbind();
        void Delete();
};

#endif // !VBO_CLASS_H