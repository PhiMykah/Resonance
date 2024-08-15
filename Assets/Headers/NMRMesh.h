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
#include "nmrgraphics.h"
}

class NMRMesh : Mesh
{
    public:
        NMRMesh(std::string file, GLenum primative = GL_TRIANGLES); //Vertices& vertices, Indices& indices, Textures& textures
        
        void Draw(Shader& shader, Camera& camera);

    private:
        // Private functions

        void NMRToVertex();
        void NMR2DToVertex();

        // Private Variables

        int sizeList[MAXDIM], qSizeList[MAXDIM], dimCount;
        float fdata[FDATASIZE];
        NMR_INT totalSize;
        int qSize, vertexCount, indexCount;
        float minVal, maxVal;
        float * mat;
        float * vertexList = (float *)NULL;
        int * indexList = (int *)NULL;
};

#endif // !NMRMESH_CLASS_H