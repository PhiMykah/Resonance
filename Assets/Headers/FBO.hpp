#ifndef FRAME_BUFFER_HEADER_H
#define FRAME_BUFFER_HEADER_H

#include <glad/glad.h>
#include <stdio.h>

#include "Shader.hpp"
#include "Camera.hpp"
#include "NMRMesh.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

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

};

class SelectionFBO : public FBO
{
    public:
        static GLuint currSel;
        void InitVAO(void * meshPtr, VAO<PosVertex> &vao);
        void InitVAO(Mesh *ptr, VAO<PosVertex> &vao);
        void SelectMesh(Shader & selection_shader, Camera & camera, std::map<std::string, void *> nmrMeshes);
        void SelectMesh(Shader & selection_shader, Camera & camera, std::vector<Mesh *> vector);
        void DrawSelection(Shader & selection_shader, Camera & camera, VAO<PosVertex> & vao, NMRMesh * mesh);
        void DrawSelection(Shader &shader, Camera &camera, VAO<PosVertex> &vao, Mesh * ptr);
    private:
};

#endif // !FRAME_BUFFER_HEADER_H