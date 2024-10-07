#include "NMRMesh.hpp"
#include "Cubemap.hpp"
#include "Shapes.hpp"

NMRMesh::NMRMesh(){

    // ***********************
    // * Create Bounding Box *
    // ***********************

    boundingBox = new Cubemap("Assets/Textures/Skybox/SolidColor/", PNG);

    boundingBox->BindTextures();

    // *************************
    // * Creating Light Object *
    // *************************

    Texture textures[]
    {
        Texture("Assets/Textures/Alb/planks.png", "diffuse", 0, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR),  // Load diffusion texture
        Texture("Assets/Textures/Spec/planksSpec.png", "specular", 1, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST) // Load Specular Map for Texture
    };

    // Create vectors for vertices, indices, and textures
    // Textures will be the same since light does not utilize textures
    Vertices light_verts(Shapes::cube_vertices, 
                         Shapes::cube_vertices + sizeof(Shapes::cube_vertices) / sizeof(Vertex));
    Indices light_ind(Shapes::cube_indices,
                      Shapes::cube_indices + sizeof(Shapes::cube_indices) / sizeof(GLuint));
    Textures tex(textures, textures + sizeof(textures) / sizeof(Texture));

    light = new Mesh(light_verts, light_ind, tex);
}

NMRMesh::~NMRMesh()
{
    delete boundingBox;
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

    NMRMesh();
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

void NMRMesh::updateUniforms(Shaders & shaders)
{
    // **************************
    // * Normal Vector Settings *
    // **************************

    shaders["normals"].Activate();
    glUniform1f(glGetUniformLocation(shaders["normals"].ID, "hairLength"), normalLength);

    // *************************
    // * Light Object Settings *
    // *************************

    // Calculate light position by parametric representation of a circle
    light_pos = glm::vec3(light_distance * cos(light_rotation), 0.5f, light_distance * sin(light_rotation));
    // Send updated light position to shader program
    glUniform3f(glGetUniformLocation(shaders["default"].ID, "lightPos"), light_pos.x, light_pos.y, light_pos.z);

    // Export shape object to shader program
    shaders["default"].Activate();
    // Export light color and position to shader program
    glUniform4f(glGetUniformLocation(shaders["default"].ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);


    // ******************
    // * Point Settings *
    // ******************

    shaders["points"].Activate();
    glUniform1f(glGetUniformLocation(shaders["points"].ID, "pointSize"), pointSize);

    // Export light object to light shader
    shaders["light"].Activate();
    GLuint light_model_ID = glGetUniformLocation(shaders["light"].ID, "model");
    glUniformMatrix4fv(light_model_ID, 1, GL_FALSE, glm::value_ptr(light_model));
    // Export light color to light shader
    glUniform4f(glGetUniformLocation(shaders["light"].ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);

}

void NMRMesh::resetAttributes()
{
    pos = ZEROS;
    scale = ONES;
    rot = QUAT_IDENTITY;
    eulerRotation = glm::eulerAngles(rot);
}

void NMRMesh::Display(WindowData &win, Camera & camera, Shaders &shaders)
{
    // *******************
    // * Main UI Drawing *
    // *******************

    // Create UI Window
    spectraUI(
        &drawShape, &drawBoundingBox, drawPoints, &pointSize, &nmrSize, &showNormals, 
        &normalLength, &light_distance, &light_rotation
        );

    // **************
    // * Draw Light *
    // **************

    light->Draw(shaders["light"], camera, light_model, pos + light_pos);

    // Draw NMRMesh
    if (drawShape){

        ImGui::Begin("Gizmo");
        ImGui::Checkbox("Show Gizmo", &showGizmo);
        if (win.width > 0 && win.height > 0) {
            drawCubeView(camera, win);
            if (showGizmo){
                EditTransform(camera, pos, rot, eulerRotation, scale, win);
            };
        }
        ImGui::End();
        
        if (drawPoints){
            SetPrimative(GL_POINTS);
            Draw(shaders["points"], camera, drawMat, pos, rot, nmrSize * scale);
        } else {
            SetPrimative(GL_TRIANGLES);
            Draw(shaders["default"], camera, drawMat, pos, rot, nmrSize * scale);
        }
        if (showNormals) {
            Draw(shaders["normals"], camera, drawMat, pos, rot, nmrSize * scale);
        }
    }
}

void NMRMesh::DisplaySecondPass(Camera & camera, Shaders &shaders)
{
    // Draw bounding box with inverted culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    if (drawBoundingBox) {
        boundingBox->Draw(shaders["projection"], camera, MAT_IDENTITY, bbPos, rot, nmrSize * bbScale);
    }
    glDisable(GL_CULL_FACE);

    // *******************************
    // * Post Processing & Rendering *
    // *******************************

    // Pass stencil test only when not equal to one
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // Disable writing to stencil mask to avoid modifying afterwards
    glStencilMask(0x00);
    // Disable depth buffer to avoid modifying
    glDisable(GL_DEPTH_TEST);
    // Enable rgb + alpha blending
    glEnable(GL_BLEND);

    stencilUI(shaders["stencil"], outline, stencil_color);
    
    // Redraw objects with post-processing
    if (drawShape){
        Draw(shaders["stencil"], camera, drawMat, pos, rot, nmrSize * scale);
    }

    // Disable RGB + Alpha blending
    glDisable(GL_BLEND);
    // Clear stencil mask and stencil test
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // Re-enable depth buffer
    glEnable(GL_DEPTH_TEST);

}
