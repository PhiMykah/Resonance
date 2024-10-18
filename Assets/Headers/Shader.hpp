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

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

        void setBool(const std::string &name, bool value) const;

        void setInt(const std::string &name, int value) const;

        void setUInt(const std::string &name, int value) const;

        void setFloat(const std::string &name, float value) const;

        void setVec2(const std::string &name, const glm::vec2 &value) const;

        void setVec2(const std::string &name, float x, float y) const;

        void setVec3(const std::string &name, const glm::vec3 &value) const;

        void setVec3(const std::string &name, float x, float y, float z) const;

        void setVec4(const std::string &name, const glm::vec4 &value) const;

        void setVec4(const std::string &name, float x, float y, float z, float w) const;

        void setMat2(const std::string &name, const glm::mat2 &mat) const;

        void setMat3(const std::string &name, const glm::mat3 &mat) const;

        void setMat4(const std::string &name, const glm::mat4 &mat) const;

    private:
        // Check if shader compilation results in any errors
        void compileErrors(unsigned int shader, const char * type);
};

typedef std::map<std::string, Shader> Shaders;

void initializeShaders(Shaders & shaders, std::string shader_path, std::vector<std::string> shader_list);

#endif // !SHADER_CLASS_H
