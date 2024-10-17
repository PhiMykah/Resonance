#include "NMRMesh.hpp"

unsigned int NMRMesh::nextID = 1;
GLuint NMRMesh::selID = 0;
ImGuizmo::OPERATION NMRMesh::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
ImGuizmo::MODE NMRMesh::mCurrentGizmoMode = ImGuizmo::WORLD;

NMRMesh::NMRMesh(){
    NMRMesh::Constructor(nextID++);
}

// NMRMesh::~NMRMesh()
// {
//     delete boundingBox;
// }

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

    NMRMesh::Constructor(nextID++);
}

void NMRMesh::Constructor(unsigned int ID)
{
    // Set Mesh ID
    NMRMesh::ID = ID;

    sprintf(IDTag, "##%d", NMRMesh::ID);

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

void NMRMesh::NMRToVertex()
{
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

const char *NMRMesh::UITxt(char *text)
{
    thread_local std::string tag;
    tag = std::string(text); // + "##" + std::to_string(selID);
    return(tag.c_str());
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

    // ********************
    // * Stencil Settings *
    // ********************
    shaders["stencil"].Activate(); // Activate stencil outline program
    glUniform1f(glGetUniformLocation(shaders["stencil"].ID, "outlining"), outline); // Collect outline thickness parameter
    glUniform4f(glGetUniformLocation(shaders["stencil"].ID, "color"), 
                stencil_color[0], stencil_color[1], stencil_color[2], stencil_color[3]);
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

    // ***********************
    // * Set Render Settings *
    // ***********************

    glRenderSettings();

    glDisable(GL_CULL_FACE);

    // Enable writing to entire stencil buffer
    //            function | ref | bitwise AND mask
    glStencilFunc(GL_ALWAYS,  1,    0xFF);
    glStencilMask(0xFF);

    // **************
    // * Draw Light *
    // **************

    light->Draw(shaders["light"], camera, light_model, pos + light_pos);

    // **************
    // * UI Drawing *
    // **************

    if (selID == ID) {
        DisplayUI(win, camera, shaders);
    }
    
    // ****************
    // * Mesh Drawing *
    // ****************

    // First Draw Pass
    if (drawShape){
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

    // ******************************************
    // * Post Processing & Additional Rendering *
    // ******************************************

    if (drawBoundingBox) {
        DisplayBoundingBox(camera, shaders);
    }

    if (selID == ID) {
        if (drawShape) {
            DisplayStencil(camera, shaders);
        }
    }

    glEnable(GL_CULL_FACE);
}

void NMRMesh::DisplayUI(WindowData &win, Camera & camera, Shaders &shaders)
{
    // std::string spec = "Spectra #";
    // spec += std::to_string(selID);
    std::string spec = "Spectra";

    if (ImGui::Begin(spec.c_str())) 
    {
        if (ImGui::BeginTabBar(UITxt("SpectraSettings")))
        {
            // Spectral Settings Tab
            if (ImGui::BeginTabItem(UITxt("Spectra")))
            {
                SpectraUI();
                ImGui::EndTabItem();
            }

            // Gizmo tab
            if (ImGui::BeginTabItem(UITxt("Gizmo")))
            {
                if (drawShape) {
                    ImGui::Checkbox(UITxt("Show Gizmo"), &showGizmo);
                    if (win.width > 0 && win.height > 0) {
                        // drawCubeView(camera, win);
                        if (showGizmo) {
                            GizmoUI();
                        }
                    }  
                }
                ImGui::EndTabItem();
            }
            if (showGizmo){
                EditTransform(camera, win);
            };
            // Stencil Tab
            if (ImGui::BeginTabItem(UITxt("Stencil")))
            {
                StencilUI();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();

}

void NMRMesh::DisplayBoundingBox(Camera & camera, Shaders &shaders)
{
    // Draw bounding box with inverted culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    boundingBox->Draw(shaders["projection"], camera, MAT_IDENTITY, bbPos, rot, nmrSize * bbScale);
    glDisable(GL_CULL_FACE);
}

void NMRMesh::DisplayStencil(Camera &camera, Shaders &shaders)
{
    // Pass stencil test only when not equal to one
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // Disable writing to stencil mask to avoid modifying afterwards
    glStencilMask(0x00);
    // Disable depth buffer to avoid modifying
    glDisable(GL_DEPTH_TEST);
    // Enable rgb + alpha blending
    glEnable(GL_BLEND);
    
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

void NMRMesh::SpectraUI(){

    ImGui::Text("Do you like this shape?");                                             // Text that appears in the window
    ImGui::Checkbox(UITxt("Draw Shape"), &drawShape);                                   // Select whether to draw the shape
    ImGui::SliderFloat(UITxt("Scale"), &nmrSize, 0.5, 5);                               // Scale Object

    ImGui::Separator();                                                                 // ------------------

    ImGui::Text("Bounding Box Settings");                                               // Text for bounding box
    ImGui::Checkbox(UITxt("Draw Bounding Box"), &drawBoundingBox);                      // Select whether to draw the bounding box

    ImGui::Separator();                                                                 // ------------------

    ImGui::Text("Display Type");                                                        // Text for drawing type
    if (ImGui::RadioButton(UITxt("Mesh"), drawPoints == false))                         // Mesh draw type
        drawPoints = false;
    ImGui::SameLine();
    if (ImGui::RadioButton(UITxt("Point Cloud"), drawPoints == true))                   // Point cloud draw type
        drawPoints = true;
    // Drawing type display settings
    if (drawPoints) {                                                                   // Display point settings
        ImGui::SliderFloat(UITxt("Point Size"), &pointSize, 0, 10);                     // Slider for point size
    }
    else {                                                                              // Display mesh settings
        ImGui::Text("Mesh Normals");
        ImGui::Checkbox(UITxt("Show Normals"), &showNormals);                           // Display normal vectors
        ImGui::SliderFloat(UITxt("Normals Magnitude"), &normalLength, 0.0f, 0.1f);      // Length of normal vectors
    }

    ImGui::Separator();                                                                 // ------------------

    ImGui::Text("Light Settings");                                                      // Text for light settings
    ImGui::SliderFloat(UITxt("Light Distance"), &light_distance, 0.5f, 5.0f);           // Slider sets distance of light from center
    ImGui::SliderAngle(UITxt("Light Rotation"), &light_rotation, 0.0f);                 // Angle on circle that light object is positioned at
}

void NMRMesh::StencilUI() {
    ImGui::Text("Change the Outline Thickness?");        // Text that appears in the window
    ImGui::SliderFloat(UITxt("Thickness"), &outline, 0.1f, 2.0f); // Size slider that appears in the window
 
    ImGui::Text("Change the Outline Color?");
    ImGui::ColorPicker4(UITxt("Color"), stencil_color);
}

void NMRMesh::GizmoUI()
{

    if (ImGui::IsKeyPressed(ImGuiKey_T)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_G)) mCurrentGizmoOperation = ImGuizmo::SCALE;

    if (ImGui::Button(UITxt("Reset"))){
        pos = ZEROS;
        scale = ONES;
        rot = QUAT_IDENTITY;
        eulerRotation = glm::eulerAngles(rot);
    }
    
    if (ImGui::RadioButton(UITxt("Translate"), mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton(UITxt("Rotate"), mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton(UITxt("Scale"), mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    ImGui::InputFloat3(UITxt("Translation"), glm::value_ptr(pos), "%.3f");
    
    // ImGui::PushItemFlag(ImGui::ImGuiItemFlags_Disabled, true);  // Disable the input field
    ImGui::InputFloat3(UITxt("Rotation"), glm::value_ptr(eulerRotation), "%.3f", ImGuiInputTextFlags_ReadOnly);
    // ImGui::PopItemFlag();  // Re-enable for the rest of the UI

    ImGui::InputFloat3(UITxt("Scale"), glm::value_ptr(scale), "%.3f");

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton(UITxt("Local"), mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton(UITxt("World"), mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

}

void NMRMesh::EditTransform(const Camera &camera, WindowData win)
{
    eulerRotation = glm::eulerAngles(rot);
    glm::mat4 modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);
    // glm::mat4 gridMatrix = glm::translate(MAT_IDENTITY, glm::vec3(0.0f, -2.0f, 0.0f));

    float matrix[16];

    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    rot = glm::quat(eulerRotation);
    modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);
    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    ImGuiIO& io = ImGui::GetIO();
    
    glm::mat4 view = MAT_IDENTITY;
    glm::mat4 projection = MAT_IDENTITY;

    view = glm::lookAt(camera.position, camera.position + camera.orientation, camera.up);
    projection = glm::perspective(glm::radians(camera.FOVdeg), (float)(float(win.width)/(float)(win.height)), camera.nearPlane, camera.farPlane);

    // ImGuizmo::DrawGrid((float *)glm::value_ptr(view), (float *)glm::value_ptr(projection), (float *)glm::value_ptr(gridMatrix), 10.f);
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(
        (float *)glm::value_ptr(view), (float *)glm::value_ptr(projection), 
        mCurrentGizmoOperation, mCurrentGizmoMode, matrix
    );
    
    // Decompose the matrix to pos, rot, scale
    glm::mat4 newMatrix = glm::make_mat4(matrix);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(newMatrix, scale, rot, pos, skew, perspective);
    
}

void MeshList(std::map<std::string, void *> nmrMeshes)
{
    if (nmrMeshes.empty()) {
        return;
    }

    ImGui::Begin("Mesh List");
    for (auto [file, meshPtr] : nmrMeshes) {
        if (!file.empty()) {
            NMRMesh * mesh = static_cast<NMRMesh *>(meshPtr);
            ImGui::Checkbox(fs::path(file).stem().string().c_str(), &mesh->drawShape);
        }
    }
    ImGui::End();
}
