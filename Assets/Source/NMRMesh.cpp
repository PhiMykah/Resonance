#include "NMRMesh.h"

NMRMesh::NMRMesh(std::string file){
    int error;
    char * inName = &file[0];

    error = readNMR( inName, NMRMesh::fdata, &mat, NMRMesh::sizeList, NMRMesh::qSizeList, &totalSize, &qSize, &dimCount);

    if (error != 0) {
        std::cout << "Error whilst reading NMR file! Error code " << error << std::endl;
        return;
    }

    NMRToVertex();
    // Consider emptying mat here since data is now in vertices
    initMesh(NMRMesh::vertices, NMRMesh::indices, NMRMesh::textures);
}

void NMRMesh::NMRToVertex(){
    switch (NMRMesh::dimCount)
    {
    case 2:
    default:
        NMR2DToVertex();
        break;
    }
};

void NMRMesh::NMR2DToVertex(){
    glm::vec3 multi_index = glm::vec3(0.0);
    glm::vec3 maxSizes = glm::vec3(sizeList[0], sizeList[1], sizeList[2]);
    glm::vec3 coordinates;
    NMR_INT indx = 0;
    int dim;
    bool iterateCompleted;

    // Iterate through entirety of mat (Total size may not include complex)
    while (indx < totalSize) {
        iterateCompleted = false;
        dim = 0;

        coordinates = glm::vec3(multi_index[0], NMRMesh::mat[indx], multi_index[1]); 
        NMRMesh::vertices.push_back(Vertex{coordinates, glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0, 1.0, 1.0)});
        NMRMesh::indices.push_back(indx);

        while (!iterateCompleted && dim < dimCount) {
            if ((multi_index[dim] + 1) < (sizeList[dim])) {
                multi_index[dim] += 1;
                iterateCompleted = true;
            }
            else {
                multi_index[dim] = 0;
                dim++;
            }
        }

        indx++;
    }
}

void NMRMesh::Draw(Shader& shader, Camera& camera){

    // Activate shader and bind vao to shader
    shader.Activate();
    vao.Bind();

    // Initialize diffuse texture and specular texture count
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    // For loop iterates over all textures, and categorizes each texture
    // as diffuse or specular
    for (unsigned int i = 0; i < textures.size(); i++) {
        std::string num;
        std::string type = textures[i].type;
        // Increment diffuse texture count if texture is diffuse
        if (type == "diffuse") {
            num = std::to_string(numDiffuse++); // Increment happens after line is run
        }
        // Increment specular texture count if texture is specular
        else if (type == "specular") {
            num = std::to_string(numSpecular++); // Increment happens after line is run
        }

        // Add texture to texUnit based on number given
        textures[i].texUnit(shader, (type + num).c_str(), i);
        // Bind the texture to shader
        textures[i].Bind();
    }
    // Send camera position to shader and perform camera matrix calculations
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
}