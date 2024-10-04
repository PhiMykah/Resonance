#ifndef NMRMESH_CLASS_H
#define NMRMESH_CLASS_H

#include <string>
#include <vector>
#include "VAO.hpp"
#include "EBO.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Constants.hpp"

extern "C" {
#include "fdatap.h"
#include "memory.h"
//#include "cmndargs.h"
//#include "specunit.h"
#include "readnmr.h"
#include "inquire.h"
#include "vutil.h"
#include "prec.h"
#include "rand.h"
//#include "atof.h"
#include "nmrgraphics.h"
}

/*
Mesh Object for NMR Data
*/
class NMRMesh : public Mesh
{
    public:
        // Empty NMRMesh Object
        NMRMesh();

        // Initialize and create a NMRMesh based on given NMR file and primative
        NMRMesh(std::string file, GLenum primative = GL_TRIANGLES); //Vertices& vertices, Indices& indices, Textures& textures
        
        // Draw NMRMesh object
        void Draw(
            Shader& shader,
            Camera& camera,
            glm::mat4 matrix = MAT_IDENTITY,
            glm::vec3 translation = ZEROS,
            glm::quat rotation = QUAT_IDENTITY,
            glm::vec3 scale = ONES
        );

    private:
        // Private functions

        // Convert NMR data to vertex coordinates
        void NMRToVertex();

        // Convert 2D NMR data to vertex coordinates
        void NMR2DToVertex();

        // Private Variables

        int sizeList[MAXDIM], qSizeList[MAXDIM], dimCount;
        float fdata[FDATASIZE];
        NMR_INT totalSize;
        int qSize, vertexCount, indexCount, normCount;
        float minVal, maxVal;
        float * mat;
        float * vertexList = (float *)NULL;
        int * indexList = (int *)NULL;
        float * normXYZ = (float *)NULL;
        std::vector<glm::vec3> normals;
        
};

#endif // !NMRMESH_CLASS_H