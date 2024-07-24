#include "EBO.h"

/*
Main constructor for EBO

Parameters
----------
indices : GLfloat *
    Pointer to index array for buffer
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
*/
EBO::EBO(GLuint * indices, GLsizeiptr size, GLenum usage){
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);
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
void EBO::Bind()
{
    // Buffer type for index buffer is GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
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
void EBO::Unbind(){
    // Buffer type for index buffer is GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
void EBO::Delete(){
    glDeleteBuffers(1, &ID);
}