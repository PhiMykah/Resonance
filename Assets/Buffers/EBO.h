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
        GLuint ID;
        EBO(GLuint* indices, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

        void Bind();
        void Unbind();
        void Delete();
};

#endif // !EBO_CLASS_H