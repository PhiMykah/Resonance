#ifndef TEXT_HEADER_H
#define TEXT_HEADER_H

#include "Backend.hpp"
#include "Shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

class Text
{
public:
    Text();
    Text(std::string text);
    void Init(std::string text);
    void Load();
    void SetText(std::string text);
    void RenderText(Shader &s, float x, float y, float scale, glm::vec3 color);
    struct Character 
    {
        unsigned int textureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };
private:
    std::map<char, Character> Characters;
    static glm::mat4 projection;
    static std::vector<GLuint> indices;
    std::string str;
    VAO<TextVertex> vao;
    VBO<TextVertex> vbo = VBO<TextVertex>(4, GL_DYNAMIC_DRAW);
    EBO ebo = EBO(indices, GL_DYNAMIC_DRAW);
};

#endif // !TEXT_HEADER_H