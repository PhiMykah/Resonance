#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

/*
### Index Buffer Object (EBO)
Class for containing and handling OpenGL index buffer
*/
class EBO
{
    public:
        // Reference ID for EBO
        GLuint ID;

        // EBO constructor for storing index array and EBO information
        EBO(GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

        // Bind EBO to binding point
        void Bind();

        // Unbind EBO from binding point
        void Unbind();

        // Delete EBO
        void Delete();
};

#endif // !EBO_CLASS_H