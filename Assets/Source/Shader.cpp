#include "Shader.hpp"

/*
Obtain the contents of a file and return a string containing the data.

Parameters
----------
filename : const char *
    Target file to read

Returns
-------
contents : std::string
    Contents of the file represented as a C++ string
*/
std::string get_file_contents(const char * filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents; // String to hold contents of the file

        // Seek to the end of the file and obtain the length of the file
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());

        // Return to the beginning of the file and read to the end
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close(); // Don't forget to close the file!

        return(contents);
    }
    return("");
}

std::string shaderFile(std::string shader_path, std::string name, ShaderType shader_type){
    std::string suffix;

    switch (shader_type)
    {
    case VERT:
        suffix = ".vert";
        break;
    case FRAG:
        suffix = ".frag";
        break;
    case GEOM:
        suffix = ".geom";
        break;
    default:
        std::string file;
        return file;
    }

    return shader_path + name + '/' + name + suffix;
}

/*
Default constructor for shader class object
*/
Shader::Shader()
{
}

/*
Main Constructor for Shader class object

Parameters
----------
vertexFile : const char *
    Filepath to vertex file
fragmentFile : const char *
    Filepath to fragment file

Returns
-------
Shader object
*/
Shader::Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile)
{
    bool useGeometry = true;

    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);
    std::string geometryCode = get_file_contents(geometryFile);

    if (geometryCode.empty()) {
        useGeometry = false;
    }

    const char* vertexSource    = vertexCode.c_str();
    const char* fragmentSource  = fragmentCode.c_str();
    const char* geometrySource  = geometryCode.c_str();
    

    // ****** VERTEX SHADER ********

    // Initialize vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Provide source to vertex shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(vertexShader, 1, &vertexSource, NULL);

    // Compile source code to machine code
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // ****** FRAGMENT SHADER *********

    // Initialize fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Provide source to fragment shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    // Compile source code to machine code
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    // ****** GEOMETRY SHADER ***********

    GLuint geometryShader;
    if (useGeometry) {
        // Initialize geometry shader
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        // Provide source for geometry shader
        // Parameters:
        // reference value, screen count, source code, array of string lengths
        glShaderSource(geometryShader, 1, &geometrySource, NULL);

        // Compile source code into machine code 
        glCompileShader(geometryShader);
        compileErrors(geometryShader, "GEOMETRY");
    }


    // ******** SHADER PROGRAM ******** 

    // Create shader program and store as the Shader class ID, attach, then wrap shaders into shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    if (useGeometry) glAttachShader(ID, geometryShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    // Remove shaders now that the shaders are loaded to the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (useGeometry) glDeleteShader(geometryShader);
}

/*
Set OpenGL to use shader

Parameters
----------
None

Returns
-------
None
*/
void Shader::Activate(){
    glUseProgram(ID);
}

/*
Set OpenGL to remove shader

Parameters
----------
None

Returns
-------
None
*/
void Shader::Delete(){
    glDeleteProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setUInt(const std::string &name, int value) const
{ 
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}


/*
Check if compiling target shader or program leads to any errors
and output error logs if applicable

Parameters
----------
shader : unsigned int
    Shader/program ID to monitor for errors

type : const char *
    Type of shader or program to output in case of an error.

Returns
-------
None
*/
void Shader::compileErrors(unsigned int shader, const char * type){
    GLint hasCompiled;
    const char * prog = "PROGRAM";
    const GLsizei logSize = 1024;
    char infoLog[logSize];
	if (type != prog){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE){
			glGetShaderInfoLog(shader, logSize, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE){
			glGetProgramInfoLog(shader, logSize, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << infoLog << std::endl;
		}
	}
}

void initializeShaders(Shaders & shaders, std::string shader_path, std::vector<std::string> shader_list){
    for (auto name : shader_list) {
        shaders.insert({name, 
            Shader(
            shaderFile(shader_path, name, VERT).c_str(),
            shaderFile(shader_path, name, FRAG).c_str(),
            shaderFile(shader_path, name, GEOM).c_str()
            )}
        );
    }
}