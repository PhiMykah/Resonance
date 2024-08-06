#include "VBO.h"

/*
Main constructor for VBO

Parameters
----------
vertices : GLfloat *
    Pointer to vertex array for buffer
size : GLsizeiptr
    Size of vertex array
usage : GLenum
    - Data usage (STREAM, STATIC, DYNAMIC)
        - STREAM suggests the data will be initialized once and used a few times
        - STATIC suggests the data will be initialized once and used many times
        - DYNAMIC suggests the data will be initialized and used many times
    - Specify data usage action (DRAW, READ, COPY)
        - DRAW suggests the data will be drawn on the screen
        - READ suggests the data will be read
        - COPY suggests that the data will be copied

Returns
-------
VBO Object
*/
VBO::VBO(std::vector<Vertex>& vertices, GLenum usage){
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), usage);
}

/*
Bind buffer to binding point

Parameters
----------
None

Returns
-------
None
*/
void VBO::Bind()
{
    // Buffer type for vertex buffer is GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

/*
Unbind buffer from binding point

Parameters
----------
None

Returns
-------
None
*/
void VBO::Unbind(){
    // Buffer type for vertex buffer is GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
Delete buffer data

Parameters
----------
None

Returns
-------
None
*/
void VBO::Delete(){
    glDeleteBuffers(1, &ID);
}
