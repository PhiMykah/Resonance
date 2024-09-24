#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "Shader.hpp"

class Texture
{
    public:
        // Texture object ID
        GLuint ID;

        // Texture's datatype
        const char * type;

        GLuint unit;

        // Constructor for texture given parameters for initialization
        Texture(const char * image, const char * texType, GLuint slot, GLenum minLOD, GLenum magLOD);

        // Assign texture to texture unit in shader
        void texUnit(Shader& shader, const char* uniform, GLuint unit);
        
        // Bind texture to binding point
        void Bind();

        // Unbind texture from binding point
        void Unbind();

        // Delete texture object
        void Delete();
};
#endif // !TEXTURE_CLASS_H