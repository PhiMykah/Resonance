#ifndef TYPE_HEADER_H
#define TYPE_HEADER_H

#include "Backend.hpp"
#include "Shader.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

class Type
{
    public:
        Type(const char * fontFile);

        static void SetProjection(float left, float right, float bottom, float top);

        static glm::f32* GetProjection();
        
        glm::vec4 RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

        void RenderCenter(Shader &shader, glm::vec2 center_point, glm::vec3 color);
        struct Character 
        {
            unsigned int textureID;  // ID handle of the glyph texture
            glm::ivec2   Size;       // Size of glyph
            glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
            unsigned int Advance;    // Offset to advance to next glyph
        };

        static glm::mat4 projection;

    private:
        // Private functions
        int LoadFont(const char * fontFile);

        int GenerateChars();

        int GenerateBuffers();

        // Private variables
        FT_Library ft;
        FT_Face face;
        static GLenum unit;
        static std::vector<GLuint> indices;
        unsigned int VAO, VBO, EBO;
        std::map<char, Character> Characters;
};

#endif // !TYPE_HEADER_H