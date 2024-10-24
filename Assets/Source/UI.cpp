#include "UI.hpp"

void DrawMainMenu(std::map<std::string, void *>& nmrFiles, std::vector<Light*> & lights, std::string & currFile, GLFWwindow * window) { // 

    // open Dialog Simple
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { 
                OpenFileDialog();
            }

            if (ImGui::BeginMenu("Add..")) {
                if (ImGui::MenuItem("Spectra", "Ctrl+A")) {
                    AddFileDialog();
                }
                if (ImGui::MenuItem("Light")) {
                    CreateLight(lights);
                }
                ImGui::EndMenu();
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
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show open dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
        // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        currFile = ImGuiFileDialog::Instance()->GetFilePathName();

        for (auto & [key, val] : nmrFiles)
        {
            if (val != NULL) {
                delete val;
                val = NULL;
            }
        }
        
        nmrFiles.clear();
        nmrFiles[currFile] = NULL;

        }

        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("AddFileDlgKey")) { // => will show add dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
        // std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
        currFile = ImGuiFileDialog::Instance()->GetFilePathName();
        nmrFiles[currFile] = NULL;

        }
        
        ImGuiFileDialog::Instance()->Close();
    }
}

void EditTransform(
    const Camera& camera, glm::vec3& pos, 
    glm::quat& rot, glm::vec3& eulerAngles, glm::vec3& scale, 
    WindowData win
    )
{
    
}

void drawCubeView(Camera & camera, WindowData win){
    ImGuizmo::ViewManipulate(glm::value_ptr(camera.view), 10, ImVec2(camera.width - 128.0f, camera.height - 128.0f), ImVec2(128.0f, 128.0f), 0x10101010);
    glm::mat4 viewInverse = glm::inverse(camera.view);

    // Extract position
    camera.position = glm::vec3(viewInverse[3]);

    // Extract orientation (camera's forward direction)
    // This is typically the negative Z-axis direction
    camera.orientation = -glm::normalize(glm::vec3(viewInverse[2]));

    // Extract the up vector
    camera.up = glm::normalize(glm::vec3(viewInverse[1]));

    camera.UpdateMatrix(win.width, win.height);
}

void CreateLight(std::vector<Light *> &lights)
{
    unsigned int ID = lights.size();
    Light * newLight;

    if (lights.size() > MAX_LIGHTS) {
        Light * oldLight = lights.at(0);
        newLight = new Light(0, LightType::POINT);
        delete oldLight;
        lights.at(0) = newLight;
        return;
    }

    newLight = new Light(ID, LightType::POINT);
    lights.push_back(newLight);

    if (lights.size() > 0) {
        Light::selID = 0;
    }
}
