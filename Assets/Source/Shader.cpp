#include "Shader.h"

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
    throw(errno);
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
Shader::Shader(const char * vertexFile, const char * fragmentFile){
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Initialize vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Provide source to vertex shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(vertexShader, 1, &vertexSource, NULL);

    // Compile source code to machine code
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // Initialize fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Provide source to fragment shader
    // Parameters:
    // reference value, screen count, source code, array of string lengths
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    // Compile source code to machine code
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    // Create shader program and store as the Shader class ID, attach, then wrap shaders into shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    // Remove shaders now that the shaders are loaded to the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
    GLsizei logSize = 1024;
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
