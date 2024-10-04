#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

// Include headers for obtaining shader source code
#include <glad/glad.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

// Other headers
#include <map>
#include <vector>
#include <string>

std::string get_file_contents(const char * filename);

// Define shader operations
enum ShaderType
{
    VERT,
    FRAG,
    GEOM
};

std::string shaderFile(std::string shader_path, std::string name, ShaderType shader_type);

/*
Object for loading vertex and fragment shaders 
by source code to a new program
*/
class Shader
{
    public:
        // Reference ID of the shader program
        GLuint ID;

        // Default constructor
        Shader();
        
        // Constructor that builds shader program from vertex and fragment shaders
        Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile);

        // Activate shader program
        void Activate();

        // Delete shader program
        void Delete();
    private:
        // Check if shader compilation results in any errors
        void compileErrors(unsigned int shader, const char * type);
};

void initializeShaders(std::map<std::string, Shader> & shaders, std::string shader_path, std::vector<std::string> shader_list);

#endif // !SHADER_CLASS_H
