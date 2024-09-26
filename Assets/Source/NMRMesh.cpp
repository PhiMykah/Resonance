#include "NMRMesh.hpp"

NMRMesh::NMRMesh(){

}

NMRMesh::NMRMesh(std::string file, GLenum primative){
    NMRMesh::primative = primative;

    int error;
    char * inName = &file[0];    
    char errorMsg[64];

    error = readNMR( inName, NMRMesh::fdata, &mat, NMRMesh::sizeList, NMRMesh::qSizeList, &totalSize, &qSize, &dimCount);

    if (error != 0) {
        sprintf(errorMsg, "Error whilst reading NMR file! Error code %d", error);
        throw std::runtime_error(errorMsg);
    }

    minVal = vecMin64(mat, totalSize);
    maxVal = vecMax64(mat, totalSize);

    error = mat2mesh(&vertexList, &vertexCount, &indexList, &indexCount, mat, qSize*sizeList[XLOC], sizeList[YLOC], minVal, maxVal, (float)0.01);

    error = findGridNormals(&normXYZ, &normCount, mat, qSize*sizeList[XLOC], sizeList[YLOC], minVal, maxVal);
    
    if (error != 0) {
        sprintf(errorMsg, "Error whilst converting to mesh! Error code %d", error);
        throw std::runtime_error(errorMsg);
    }

    NMRToVertex();

    NMRMesh::textures.push_back(
        Texture("Assets/Textures/Alb/3f4647ff.png", "diffuse", 0, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR) // Load diffusion texture
    );

    NMRMesh::textures.push_back(
        Texture("Assets/Textures/Spec/FFFFFFFF.png", "specular", 1, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST) // Load Specular Map for Texture
    );

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
    Vertex newVert;
    for (int i = 0; i < indexCount; i++)
    {
        newVert.position = 
            glm::vec3(vertexList[indexList[i]],
            vertexList[indexList[i] + 1],
            vertexList[indexList[i] + 2]);

        newVert.normal =
            glm::vec3(normXYZ[indexList[i]],
                normXYZ[indexList[i] + 1],
                normXYZ[indexList[i] + 2]);

        newVert.color = 
            glm::vec3(1.0f, 1.0f, 1.0f);

        newVert.texUV = glm::vec2(0.0, 0.0);

        NMRMesh::vertices.push_back(newVert);

        if (i >= 0) {
            NMRMesh::indices.push_back((GLuint) i);
        } else {
            NMRMesh::indices.push_back((GLuint) -1 * i);
        }
    }
    
}

/*
    {
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
*/
void NMRMesh::Draw(
    Shader& shader, Camera& camera, 
    glm::mat4 matrix, glm::vec3 translation,
    glm::quat rotation, glm::vec3 scale
    ){
    Mesh::Draw(shader, camera, matrix, translation, rotation, scale);
    /*
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
    */
}