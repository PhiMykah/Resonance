#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

// C/C++ Headers
#include <iostream>
#include <string>

// OpenGL processing headers
#include <glad/glad.h>
#include <stb/stb_image.h>

// Object Headers
#include "Shader.h"
#include "Camera.h"

// Matrix Operation Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Definitions
extern float vertices[];
extern unsigned int indices[];

typedef int FileType;
#define PNG 0x251B
#define JPG 0x251C

class Cubemap
{
    public:
        GLuint VAO, VBO, EBO;

        GLuint cubeMapTexture;
        FileType format;
        Cubemap(const char* cubeDir, FileType format = PNG);

        void BindTextures();
        void DrawSkybox(Shader & shader, Camera & camera, int width, int height);
    private:
        std::string faces[6];
};

#endif // !CUBEMAP_CLASS_H