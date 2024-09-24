#include "VAO.hpp"

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
vbo : VBO&
    Target vertex buffer object to link to vertex array
layout : GLuint
    Specifies the component layout location for the vertex data.
numComponents : GLint
    Number of components per layout and vertex, by default 3
type : GLenum
    Datatype of the layout components
stride : Lsizeiptr
    Distance in bytes between each vertex, by default 0
offset : void *
    Offset pointer of a layout from start of vertex in bytes
    (for example the offset between coordinate data and color data)

Returns
-------
VAO Object
*/
void VAO::LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents,
                    GLenum type, GLsizeiptr stride, void* offset){

    vbo.Bind(); // Bind vbo to binding point for use

    // Configure VAO with VBO
    // - Position of Vertex Attribute
    // - Number of Components per Layout
    // - Component type
    // - Identify whether or not values are integers
    // - Size of layout data per vertex or
    //   distance in bytes between each layout in vertex
    // - Offset from start of datastream
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
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
