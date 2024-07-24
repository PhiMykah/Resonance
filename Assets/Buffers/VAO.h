#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

/*
### Vertex Array Object (VAO)
Class for containing and handling OpenGL vertex array
*/
class VAO
{
public:
    GLuint ID;
    VAO();

    void LinkVBO(VBO vbo, GLuint layout, GLint size = 3);

    void Bind();
    void Unbind();
    void Delete();
};

#endif // !VAO_CLASS_H