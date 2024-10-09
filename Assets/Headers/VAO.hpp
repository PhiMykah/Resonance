#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.hpp"

/*
### Vertex Array Object (VAO)
Class for containing and handling OpenGL vertex array
*/
template <typename Vert> class VAO
{
public:
    // Reference ID for VAO
    GLuint ID;

    // Default VAO constructor
    VAO();

    // Link vertex layout attribute to VAO
    void LinkAttrib (VBO<Vert>& vbo, GLuint layout, GLuint numComponents = 3, 
                    GLenum type = GL_FLOAT, GLsizeiptr stride = 0, void* offset = (void*)0);

    // Bind VAO to binding point
    void Bind();

    // Unbind VAO from binding point
    void Unbind();

    // Delete VAO
    void Delete();
};

#endif // !VAO_CLASS_H