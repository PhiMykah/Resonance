#ifndef SHAPES_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

// struct Shape {
//     GLfloat * vertices;
//     GLuint * indices;
//     Shape(GLfloat * vertices_array, GLuint * indices_array)
//     {
//         vertices = vertices_array;
//         indices = indices_array;
//     }
// };

namespace Shapes {

// *******************
// * Define Triangle *
// *******************

// Initialize triangle vertex array
GLfloat triangle_vertices[] =
{ //        X,Y,Z Coordinates                       /       RGB Color        //
    -0.5f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f, // Bottom left corner
     0.5f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f, // Bottom right corner
     0.0f,   0.5f * float(sqrt(3)) * 2 / 3, 0.0f,       1.00f, 0.60f, 0.32f, // Top vertex
    -0.25f,  0.5f * float(sqrt(3)) / 6,     0.0f,       0.90f, 0.45f, 0.17f, // Mid left point
     0.25f,  0.5f * float(sqrt(3)) / 6,     0.0f,       0.90f, 0.45f, 0.17f, // Mid right Point
     0.0f,  -0.5f * float(sqrt(3)) / 3,     0.0f,       0.80f, 0.30f, 0.02f  // Bottom Center point
};

// Initialize Index buffer to tell vertex shader the order to form primitives
GLuint triangle_indices[] =
{
    0, 3, 5, // Lower left triangle uses vertices 0, 3, and 5
    3, 2, 4, // Lower right triangle uses vertices 3, 2, and 4
    5, 4, 1, // Top triangle uses vertices 5, 4, and 1
};

// *****************
// * Define Square *
// *****************

GLfloat square_vertices[] =
{ //     X,Y,Z Coordinates      /       RGB Color        /    Texture Coordinates    //
    -0.5f,  -0.5f,     0.0f,       1.00f, 0.00f, 0.00f,       0.00f, 0.00f,          // Bottom left corner
    -0.5f,   0.5f,     0.0f,       0.00f, 1.00f, 0.00f,       0.00f, 1.00f,          // Top left corner
     0.5f,   0.5f,     0.0f,       0.00f, 0.00f, 1.00f,       1.00f, 1.00f,          // Top right corner
     0.5f,  -0.5f,     0.0f,       1.00f, 1.00f, 1.00f,       1.00f, 0.00f           // Bottom right corner
};

GLuint square_indices[] =
{
    0, 2, 1, // Upper triangle
    0, 3, 2  // Lower triangle
};

// ******************
// * Define Pyramid *
// ******************

// Vertices are duplicated since the normals will be different on each side of the pyramid.
GLfloat pyramid_vertices[] =
{ //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side

	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side

	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
};

GLuint pyramid_indices[] =
{
	0, 1, 2, // Bottom side
	0, 2, 3, // Bottom side
	4, 6, 5, // Left side
	7, 9, 8, // Non-facing side
	10, 12, 11, // Right side
	13, 15, 14 // Facing side
};

} // namespace Shapes

#endif // !SHAPES_H