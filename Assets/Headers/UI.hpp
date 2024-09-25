#include "Backend.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

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
    const Camera& camera, glm::mat4& matrix, 
    glm::vec3& pos, glm::quat& rot, glm::vec3& scale, 
    WindowData win, float FOVdeg, float nearPlane, float farPlane
    )
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(ImGuiKey_T))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_G))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(matrix), glm::value_ptr(pos), 
        glm::value_ptr(rot), glm::value_ptr(scale)
        );

    ImGui::InputFloat3("Tr", glm::value_ptr(pos), "%.3f");
    ImGui::InputFloat3("Rt", glm::value_ptr(rot), "%.3f");
    ImGui::InputFloat3("Sc", glm::value_ptr(scale), "%.3f");

    ImGuizmo::RecomposeMatrixFromComponents(
        glm::value_ptr(pos), glm::value_ptr(rot), 
        glm::value_ptr(scale), glm::value_ptr(matrix)
        );

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
        useSnap = !useSnap;
    ImGui::Checkbox("##", &useSnap);
    ImGui::SameLine();
    // vec_t snap;
    // switch (mCurrentGizmoOperation)
    // {
    // case ImGuizmo::TRANSLATE:
    //     snap = config.mSnapTranslation;
    //     ImGui::InputFloat3("Snap", &snap.x);
    //     break;
    // case ImGuizmo::ROTATE:
    //     snap = config.mSnapRotation;
    //     ImGui::InputFloat("Angle Snap", &snap.x);
    //     break;
    // case ImGuizmo::SCALE:
    //     snap = config.mSnapScale;
    //     ImGui::InputFloat("Scale Snap", &snap.x);
    //     break;
    // }
    ImGuiIO& io = ImGui::GetIO();
    
    glm::mat4 view = glm::mat4(1.0);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(camera.position, camera.position + camera.orientation, camera.up);
    projection = glm::perspective(glm::radians(FOVdeg), (float)(float(win.width)/(float)(win.height)), nearPlane, farPlane);

    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(
        (float *)glm::value_ptr(view), (float *)glm::value_ptr(projection), 
        mCurrentGizmoOperation, mCurrentGizmoMode, (float *)glm::value_ptr(matrix)
    );

    ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(matrix), glm::value_ptr(pos), 
        glm::value_ptr(rot), glm::value_ptr(scale)
        );
}

void spectraUI(
    bool * drawShape, float * nmrSize, 
    bool * showNormals, float * normalLength, float * light_distance,
    float * light_rotation){
    // Create UI Window
    ImGui::Begin("Spectra");                // ImGUI window creation
    ImGui::Text("Do you like this shape?");        // Text that appears in the window
    ImGui::Checkbox("Draw Shape", drawShape);     // Select whether to draw the shape
    ImGui::SliderFloat("Scale", nmrSize, 0.5, 5); // Scale Object
    ImGui::Checkbox("Show Normals", showNormals); // Display normal vectors
    ImGui::SliderFloat("Normals Magnitude", normalLength, 0.0f, 0.1f); // Length of normal vectors
    // ImGui::SliderFloat("Size", &size, 0.5f, 2.0f); // Size slider that appears in the window
    ImGui::SliderFloat("Light Distance", light_distance, 0.5f, 5.0f); // Slider sets distance of light from center
    ImGui::SliderAngle("Light Rotation", light_rotation, 0.0f); // Angle on circle that light object is positioned at
    // Complete UI Window definition
    ImGui::End();
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
