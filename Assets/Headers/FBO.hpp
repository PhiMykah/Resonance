#ifndef FRAME_BUFFER_HEADER_H
#define FRAME_BUFFER_HEADER_H

#include <glad/glad.h>
#include <stdio.h>

#include "Shader.hpp"
#include "Camera.hpp"
#include "NMRMesh.hpp"
#include "VAO.hpp"

/*
### Frame Buffer Object (FBO)
Class for defining and handling custom OpenGL frame buffers
*/
class FBO
{
    public:
        // Reference ID for FBO
        GLuint ID;

        // Default FBO constructor
        FBO();

        void Init(int width, int height);

        void Bind();

        void Unbind();
        
        void Delete();

        void Resize(int newWidth, int newHeight);

        struct Pixel
        {
            GLuint objID = 0;
            GLuint drawID = 0;
            GLuint PrimID = 0;

            void Print()
            {
                printf("Object %d draw %d prim %d\n", objID, drawID, PrimID);
            }
        };

        Pixel ReadPixel(GLuint x, GLuint y);
        
    private:
        // Reference ID for Frame Buffer texture
        GLuint texID;

        // Reference ID for Frame Buffer depth
        GLuint depthID;

    protected:
        VAO vao;

};

class SelectionFBO : public FBO
{
    public:
        void SelectMesh(Shader & selection_shader, Camera & camera, std::vector<NMRMesh> nmrMeshes);
    private:
};

#endif // !FRAME_BUFFER_HEADER_H