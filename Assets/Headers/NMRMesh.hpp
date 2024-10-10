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
#include "UI.hpp"
#include "Cubemap.hpp"
#include "Shapes.hpp"

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
        // Default constructor
        NMRMesh();

        // Destructor
        // ~NMRMesh();

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

        void updateUniforms(Shaders & shaders);

        void resetAttributes();
        
        // Display object instance
        void Display(WindowData &win, Camera & camera, Shaders &shaders);

        // Display all UI elements
        void DisplayUI(WindowData &win, Camera & camera, Shaders &shaders);

        // Bounding Box Display
        void DisplayBoundingBox(Camera & camera, Shaders &shaders);

        // Stencil Display
        void DisplayStencil(Camera & camera, Shaders &shaders);
        
        // Spectral settings UI
        void SpectraUI();

        // Stencil Settings UI
        void StencilUI();

        // Gizmo Settings UI
        void GizmoUI();
        
        // Edit transform gizmo
        void EditTransform(const Camera& camera, WindowData win);

        // Public variables
        
        unsigned int ID; // Unique NMRMesh object ID
        static GLuint selID; // ID of selected object

        // Display Attributes

        bool drawShape = true; 
        bool drawBoundingBox = false;
        bool drawPoints = false;
        
        bool showNormals = false;
        bool showGizmo = true;

        float pointSize = 1.0f;
        float nmrSize = 1.0f;
        float normalLength = 0.05f;

        // Mesh attributes
        glm::mat4 drawMat = glm::mat4(1.0);
        glm::vec3 pos = ZEROS;
        glm::quat rot = QUAT_IDENTITY;
        glm::vec3 eulerRotation = glm::eulerAngles(rot);
        glm::vec3 scale = ONES;

    private:
        // Private functions

        void Constructor(unsigned int ID);

        // Convert NMR data to vertex coordinates
        void NMRToVertex();

        // Convert 2D NMR data to vertex coordinates
        void NMR2DToVertex();

        // Creates ImGuiUI text with ID tag
        const char * UITxt(char * text);

        // Private Variables

        static unsigned int nextID; // next ID of NMRMesh object
        char IDTag [15]; // Tag for ID
        static ImGuizmo::OPERATION mCurrentGizmoOperation;
        static ImGuizmo::MODE mCurrentGizmoMode;

        // Cubelight 
        Mesh * light = NULL;

        // Bounding Box
        Cubemap * boundingBox = NULL; 

        // Light attributes
        glm::vec4 light_color = WHITE;
        glm::mat4 light_model = MAT_IDENTITY;
        glm::vec3 light_pos;
        float light_distance = 0.8f;
        float light_rotation = 0.0f;

        // BoundingBox attributes
        glm::vec3 bbPos = ZEROS;
        glm::vec3 bbScale = glm::vec3(2.0);

        // Stencil attributes
        float outline = 0.50f; // Stencil buffer outline
        float stencil_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Stencil buffer color

        // NMR variables
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

void MeshList(std::map<std::string, void *> nmrMeshes);

#endif // !NMRMESH_CLASS_H