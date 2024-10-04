#include "Backend.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Constants.hpp"

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

void drawMainMenu(std::string& file, GLFWwindow * window) { //

    // open Dialog Simple
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { 
                OpenFileDialog();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Toggle Fullscreen", "Alt+Enter")) {
                ToggleFullscreen(window);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    // if (ImGui::Begin("Resonance", (bool *)0, ImGuiWindowFlags_MenuBar)) {
    //     if (ImGui::BeginMenuBar())
    //     {
    //         if (ImGui::BeginMenu("File"))
    //         {
    //             if (ImGui::MenuItem("Open..", "Ctrl+O")) { 
    //                 IGFD::FileDialogConfig config;
    //                 config.path = ".";
    //                 ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*,.fid,.ft2,.ft3,.ft4", config);
    //             }
    //             ImGui::EndMenu();
    //         }
    //         ImGui::EndMenuBar();
    //     }
    // }
    // ImGui::End();

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
        // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        file = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        
        ImGuiFileDialog::Instance()->Close();
    }
}

void EditTransform(
    const Camera& camera, glm::vec3& pos, 
    glm::quat& rot, glm::vec3& eulerAngles, glm::vec3& scale, 
    WindowData win, float FOVdeg, float nearPlane, float farPlane
    )
{
    eulerAngles = glm::eulerAngles(rot);
    glm::mat4 modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);
    // glm::mat4 gridMatrix = glm::translate(MAT_IDENTITY, glm::vec3(0.0f, -2.0f, 0.0f));

    float matrix[16];

    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (ImGui::IsKeyPressed(ImGuiKey_T)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_G)) mCurrentGizmoOperation = ImGuizmo::SCALE;

    if (ImGui::Button("Reset")){
        pos = ZEROS;
        scale = ONES;
        rot = QUAT_IDENTITY;
        eulerAngles = glm::eulerAngles(rot);
    }
    
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    ImGui::InputFloat3("Translation", glm::value_ptr(pos), "%.3f");
    
    // ImGui::PushItemFlag(ImGui::ImGuiItemFlags_Disabled, true);  // Disable the input field
    ImGui::InputFloat3("Rotation", glm::value_ptr(eulerAngles), "%.3f", ImGuiInputTextFlags_ReadOnly);
    // ImGui::PopItemFlag();  // Re-enable for the rest of the UI

    ImGui::InputFloat3("Scale", glm::value_ptr(scale), "%.3f");

    rot = glm::quat(eulerAngles);
    modelMatrix = glm::translate(MAT_IDENTITY, pos) * glm::mat4_cast(rot) * glm::scale(MAT_IDENTITY, scale);
    memcpy(matrix, glm::value_ptr(modelMatrix), sizeof(float) * 16);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    ImGuiIO& io = ImGui::GetIO();
    
    glm::mat4 view = MAT_IDENTITY;
    glm::mat4 projection = MAT_IDENTITY;

    view = glm::lookAt(camera.position, camera.position + camera.orientation, camera.up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)(float(win.width)/(float)(win.height)), nearPlane, farPlane);

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

void drawCubeView(Camera & camera, WindowData win, float FOVdeg, float nearPlane, float farPlane){
    ImGuizmo::ViewManipulate(glm::value_ptr(camera.view), 10, ImVec2(camera.width - 128, camera.height - 128), ImVec2(128, 128), 0x10101010);
    glm::mat4 viewInverse = glm::inverse(camera.view);

    // Extract position
    camera.position = glm::vec3(viewInverse[3]);

    // Extract orientation (camera's forward direction)
    // This is typically the negative Z-axis direction
    camera.orientation = -glm::normalize(glm::vec3(viewInverse[2]));

    // Extract the up vector
    camera.up = glm::normalize(glm::vec3(viewInverse[1]));

    camera.UpdateMatrix(win.width, win.height, FOVdeg, nearPlane, farPlane);
}

void spectraUI(
        bool * drawShape, bool * drawBB, bool &drawPoints, float * pointSize,
        float * nmrSize, bool * showNormals, float * normalLength, 
        float * light_distance, float * light_rotation
    ){
    // Create UI Window
    ImGui::Begin("Spectra");                                                // ImGUI window creation
    ImGui::Text("Do you like this shape?");                                 // Text that appears in the window
    ImGui::Checkbox("Draw Shape", drawShape);                               // Select whether to draw the shape
    ImGui::SliderFloat("Scale", nmrSize, 0.5, 5);                           // Scale Object

    ImGui::Separator();                                                     // ------------------

    ImGui::Text("Bounding Box Settings");                                   // Text for bounding box
    ImGui::Checkbox("Draw Bounding Box", drawBB);                           // Select whether to draw the bounding box

    ImGui::Separator();                                                     // ------------------

    ImGui::Text("Display Type");                                            // Text for drawing type
    if (ImGui::RadioButton("Mesh", drawPoints == false))                    // Mesh draw type
        drawPoints = false;
    ImGui::SameLine();
    if (ImGui::RadioButton("Point Cloud", drawPoints == true))              // Point cloud draw type
        drawPoints = true;
    // Drawing type display settings
    if (drawPoints) {                                                       // Display point settings
        ImGui::SliderFloat("Point Size", pointSize, 0, 10);                 // Slider for point size
    }
    else {                                                                  // Display mesh settings
        ImGui::Text("Mesh Normals");
        ImGui::Checkbox("Show Normals", showNormals);                       // Display normal vectors
        ImGui::SliderFloat("Normals Magnitude", normalLength, 0.0f, 0.1f);  // Length of normal vectors
    }

    ImGui::Separator();                                                     // ------------------

    ImGui::Text("Light Settings");                                          // Text for light settings
    ImGui::SliderFloat("Light Distance", light_distance, 0.5f, 5.0f);       // Slider sets distance of light from center
    ImGui::SliderAngle("Light Rotation", light_rotation, 0.0f);             // Angle on circle that light object is positioned at

    ImGui::End();                                                           // End of ImGuiUI Window
}

void stencilUI(Shader & shader, float & outline, float color[4]) {
    ImGui::Begin("Stencil Settings");                // ImGUI window creation
    ImGui::Text("Change the Outline Thickness?");        // Text that appears in the window
    ImGui::SliderFloat("Thickness", &outline, 0.00f, 2.0f); // Size slider that appears in the window
 
    ImGui::Text("Change the Outline Color?");
    ImGui::ColorPicker4("Color", color);
    ImGui::End();

    shader.Activate(); // Activate stencil outline program
    glUniform1f(glGetUniformLocation(shader.ID, "outlining"), outline); // Collect outline thickness parameter
    glUniform4f(glGetUniformLocation(shader.ID, "color"), color[0], color[1], color[2], color[3]);
}
