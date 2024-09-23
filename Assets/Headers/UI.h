#include "Backend.h"
#include "Shader.h"

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

void spectraUI(
    bool * drawShape, glm::vec3 * nmrPos, float * nmrSize, 
    bool * showNormals, float * normalLength, float * light_distance,
    float * light_rotation){
    // Create UI Window
    ImGui::Begin("Spectra");                // ImGUI window creation
    ImGui::Text("Do you like this shape?");        // Text that appears in the window
    ImGui::Checkbox("Draw Shape", drawShape);     // Select whether to draw the shape
    ImGui::InputFloat3("Translation", glm::value_ptr(*nmrPos)); // Move Object
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
