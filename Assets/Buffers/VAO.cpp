#include "VAO.h"

/*
Default Constructor for vertex array class
*/
VAO::VAO()
{
    glGenVertexArrays(1, &ID);
}

/*
Link vertex buffer object to this vertex array with given layout

Parameters
----------
vbo : VBO
    Target vertex buffer object to link to vertex array
layout : GLuint
    Specifies the index of the generic vertex attribute to be modified.
size : GLint
    Number of points per vertex attribute (Default 3 for (X,Y,Z))

Returns
-------
None
*/
void VAO::LinkVBO(VBO vbo, GLuint layout, GLint size){

    vbo.Bind(); // Bind vbo to binding point for use

    // Configure VAO with VBO
    // - Position of Vertex Attribute
    // - Number of Values per Vertex
    // - Value type
    // - Identify whether or not values are integers
    // - Size of data per vertex
    // - Offset
    glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Enable Vertex Attribute Array
    glEnableVertexAttribArray(layout);

    vbo.Unbind(); // Unbind vbo once completed
}

/*
Bind vertex array to binding point

Parameters
----------
None

Returns
-------
None
*/
void VAO::Bind()
{
    glBindVertexArray(ID);
}

/*
Unbind vertex array from binding point

Parameters
----------
None

Returns
-------
None
*/
void VAO::Unbind(){
    glBindVertexArray(0);
}

/*
Delete vertex array data

Parameters
----------
None

Returns
-------
None
*/
void VAO::Delete(){
    glDeleteVertexArrays(1, &ID);
}