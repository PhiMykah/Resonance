#ifndef SHAPES_H
#define SHAPES_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "Mesh.h"

namespace Shapes {

// *******************
// * Define Triangle *
// *******************

float root3_3 = float(sqrt(3)) / 3.0f; // sqrt(3) / 3

// Initialize triangle vertex array
Vertex triangle_vertices[] =
{ //               COORDINATES           /            NORMALS         /           COLORS         /       TEXTURE COORDINATES    //
    Vertex{glm::vec3(-0.50f, -0.5f * 	 root3_3,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.80f,  0.30f,  0.02f), glm::vec2( 0.00f, 0.00f)}, // Bottom left corner
    Vertex{glm::vec3( 0.50f, -0.5f *     root3_3,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.80f,  0.30f,  0.02f), glm::vec2( 1.00f, 0.00f)}, // Bottom right corner
    Vertex{glm::vec3( 0.00f,  0.5f * 2 * root3_3,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 1.00f,  0.60f,  0.32f), glm::vec2( 0.50f, 1.00f)}, // Top vertex
    Vertex{glm::vec3(-0.25f,  0.5f * root3_3 / 2,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.90f,  0.45f,  0.17f), glm::vec2( 0.25f, 0.50f)}, // Mid left point
    Vertex{glm::vec3( 0.25f,  0.5f * root3_3 / 2,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.90f,  0.45f,  0.17f), glm::vec2( 0.75f, 0.50f)}, // Mid right Point
    Vertex{glm::vec3( 0.00f, -0.5f *	 root3_3,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.80f,  0.30f,  0.02f), glm::vec2( 0.50f, 0.00f)}  // Bottom Center point
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

Vertex square_vertices[] =
{ //               COORDINATES           /            NORMALS         /           COLORS         /       TEXTURE COORDINATES    //
    Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 1.00f, 0.00f, 0.00f), glm::vec2( 0.00f, 0.00f)},          // Bottom left corner
    Vertex{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 0.00f, 1.00f, 0.00f), glm::vec2( 0.00f, 1.00f)},          // Top left corner
    Vertex{glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 0.00f, 0.00f, 1.00f), glm::vec2( 1.00f, 1.00f)},          // Top right corner
    Vertex{glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 1.00f, 1.00f, 1.00f), glm::vec2( 1.00f, 0.00f)}           // Bottom right corner
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
Vertex pyramid_vertices[] =
  
{ //               COORDINATES           /            NORMALS         /           COLORS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, -1.0f, 0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 0.0f)}, // Bottom side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, -1.0f, 0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 5.0f)}, // Bottom side
	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, -1.0f, 0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 5.0f)}, // Bottom side
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, -1.0f, 0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 0.0f)}, // Bottom side

	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(-0.8f, 0.5f,  0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 0.0f)}, // Left Side
	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(-0.8f, 0.5f,  0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 0.0f)}, // Left Side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3(-0.8f, 0.5f,  0.0f), glm::vec3(0.92f, 0.86f, 0.76f), glm::vec2(2.5f, 5.0f)}, // Left Side

	Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, 0.5f, -0.8f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 0.0f)}, // Non-facing side
	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3( 0.0f, 0.5f, -0.8f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 0.0f)}, // Non-facing side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 0.0f, 0.5f, -0.8f), glm::vec3(0.92f, 0.86f, 0.76f), glm::vec2(2.5f, 5.0f)}, // Non-facing side

	Vertex{glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3( 0.8f, 0.5f,  0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 0.0f)}, // Right side
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3( 0.8f, 0.5f,  0.0f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 0.0f)}, // Right side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 0.8f, 0.5f,  0.0f), glm::vec3(0.92f, 0.86f, 0.76f), glm::vec2(2.5f, 5.0f)}, // Right side
	
	Vertex{glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, 0.5f,  0.8f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(5.0f, 0.0f)}, // Facing side
	Vertex{glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3( 0.0f, 0.5f,  0.8f), glm::vec3(0.83f, 0.70f, 0.44f), glm::vec2(0.0f, 0.0f)}, // Facing side
	Vertex{glm::vec3( 0.0f, 0.8f,  0.0f), glm::vec3( 0.0f, 0.5f,  0.8f), glm::vec3(0.92f, 0.86f, 0.76f), glm::vec2(2.5f, 5.0f)}  // Facing side
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


// ****************
// * Define Plane *
// ****************
Vertex plane_vertices[] = 
{ //               COORDINATES           /            NORMALS         /           COLORS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

GLuint plane_indices[] = 
{
    0, 1, 2,
    0, 2, 3
};


// ***************
// * Define Cube *
// ***************
Vertex cube_vertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint cube_indices[] =
{
    0, 1, 2, // 		Clockwise triangle (Bottom 1)
    0, 2, 3, // 		Clockwise triangle (Bottom 2)
    0, 7, 4, // Counter-Clockwise triangle (Front 1)
    0, 3, 7, // Counter-Clockwise triangle (Front 2)
    3, 6, 7, // Counter Clockwise triangle (Right 1)
    3, 2, 6, // Counter-Clockwise triangle (Right 2)
    2, 5, 6, // 		Clockwise triangle (Back 1)
    2, 1, 5, //			Clockwise triangle (Back 2)
    1, 4, 5, //			Clockwise triangle (Left 1)
    1, 8, 4, //			Clockwise triangle (Left 2)
    4, 6, 5, // Counter-Clockwise triangle (Top 1)
    4, 7, 6  // Counter-Clockwise triangle (Top 2)
};


// ********************
// * Define Rectangle *
// ********************
float rectangle_vertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

// ******************
// * Define Cubemap *
// ******************

float cubemap_vertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int cubemap_indices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};
} // namespace Shapes

#endif // !SHAPES_H