#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "../Shaders/shaderClass.h"

class Texture
{
    public:
        // Texture object ID
        GLuint ID;

        // Texture's datatype
        GLenum type;

        // Constructor for texture given parameters for initialization
        Texture(const char * image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType, GLenum minLOD, GLenum magLOD);

        // Assign texture to texture unit in shader
        void texUnit(Shader shader, const char* uniform, GLuint unit);
        
        // Bind texture to binding point
        void Bind();

        // Unbind texture from binding point
        void Unbind();

        // Delete texture object
        void Delete();
};
#endif // !TEXTURE_CLASS_H