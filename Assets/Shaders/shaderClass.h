#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H 

// Include headers for obtaining shader source code
#include <glad/glad.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char * filename);

/*
Object for loading vertex and fragment shaders by source code to 
a new program
*/
class Shader
{
    public:
        // Reference ID of the shader program
        GLuint ID;

        // Constructor that builds shader program from vertex and fragment shaders
        Shader(const char* vertexFile, const char* fragmentFile);

        // Activate shader program
        void Activate();

        // Delete shader program
        void Delete();
    private:
        // Check if shader compilation results in any errors
        void compileErrors(unsigned int shader, const char * type);
};

#endif // !SHADER_CLASS_H