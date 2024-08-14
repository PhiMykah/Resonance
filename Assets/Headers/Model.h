#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "json/json.hpp"
#include "Mesh.h"

using json = nlohmann::json;

class Model
{
public:
    // Load model from file data
    Model(const char* file);

    // Draw model using provided shader and camera
    void Draw(Shader& shader, Camera & camera);
private:
    const char* file; // File containing model data
    std::vector<unsigned char> data; // Data for model stored in bytes
    json JSON; // json parsed information for model

    // Vector containing meshes that correspond to the model
    std::vector<Mesh> meshes;

    // Vectors containing tranformation data for each mesh 
    std::vector<glm::vec3> meshTranslations;
    std::vector<glm::quat> meshRotations;
    std::vector<glm::vec3> meshScales;
    std::vector<glm::mat4> meshMatrices;
    
    // Vector containing names of already existing textures to avoid duplicate textures
    std::vector<std::string> loadedTexNames; 
    // Vector containing all texture objects already loaded to avoid duplicate textures
    Textures loadedTex;

    // Load a mesh to the meshes container with given index
    void loadMesh(unsigned int indMesh);

    // Traverse Node and obtain information of object position as traversal occurs
    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    // Use the json file to identify location of data
    // Then obtain the data and represent it as bytes
    std::vector<unsigned char> getData();

    // Obtain float values from the json hierarchy
    std::vector<float> getFloats(json accessor);

    // Obtain the indices from the json hierarchy
    Indices getIndices(json accessor);
    
    // Obtain the texture data
    Textures getTextures();
    
    // Assemble all floats into vertices
    Vertices assembleVertices
    (
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    // Group floats into vertices
    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};

#endif // !MODEL_CLASS_H