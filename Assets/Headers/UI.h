#include "Backend.h"
#include "Shader.h"

void drawFileDialog(std::string& file) { //
    // open Dialog Simple
    if (ImGui::Begin("File")) {
        if (ImGui::Button("Load NMR File")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*,.fid,.ft2,.ft3,.ft4", config);
        }
    }
    ImGui::End();

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
        // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        file = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        
        ImGuiFileDialog::Instance()->Close();
    }
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
