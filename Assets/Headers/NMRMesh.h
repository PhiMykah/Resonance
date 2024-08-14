#ifndef NMRMESH_CLASS_H
#define NMRMESH_CLASS_H

#include <string>
#include <vector>
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"

extern "C" {
#include "fdatap.h"
#include "memory.h"
#include "cmndargs.h"
#include "specunit.h"
#include "readnmr.h"
#include "inquire.h"
#include "vutil.h"
#include "prec.h"
#include "rand.h"
#include "atof.h"
}

#define Vertices std::vector<Vertex>
#define Indices std::vector<GLuint>
#define Textures std::vector<Texture>

class NMRMesh : Mesh
{
    public:
        float * mat;
        GLenum primative = GL_POINTS;
        float fdata[FDATASIZE];
        int sizeList[MAXDIM], qSizeList[MAXDIM], dimCount, qSize;
        NMR_INT totalSize;

        NMRMesh(std::string file); //Vertices& vertices, Indices& indices, Textures& textures
        
        void Draw(Shader& shader, Camera& camera);

    private:
        void NMRToVertex();
        void NMR2DToVertex();
};

#endif // !NMRMESH_CLASS_H